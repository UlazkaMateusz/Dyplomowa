<?php
namespace Upload {
    const AVRDUDE_SEMAPHORE = 5020;

    class UploadException extends \Exception {
        public $error_type;
        public $error_msg;
        public function __construct($error_type, $error_msg) {
            parent::__construct();

            $this->error_type = $error_type;
            $this->error_msg = nl2br($error_msg);
        }
    }

    function handle() {
        try {
            tryToFlashController();
            include "succes.html";
        } catch (UploadException $e) {
            include "error.php";
        }
    }

    function tryToFlashController() {
        $filename = loadFile();

        rename($filename, $filename . '.xml');
        parseXML($filename);
        transformFile($filename);
        compileFile($filename);
        createHex($filename);
        flash($filename);
        clear($filename);

    }

    function loadFile() {
        if(isset($_FILES['musicfile'])) {
            return $_FILES['musicfile']['tmp_name'];
        } else {
            throw new UploadException('wczytywania pliku', 'Nie udało się wczytać pliku.');
        }

    }

    function parseXML($filename) {
        $out = shell_exec("xsltproc --novalid parser.xslt {$filename}.xml > {$filename}.c 2>&1");
        if($out) {
            throw new UploadException('parsowania pliku', $out);
        }
    }

    function transformFile($filename) {
        $file = fopen("{$filename}.c", "r");
        $new_file_contents = '';
        if ($file) {
            while (($line = fgets($file)) !== false) {
                $new_file_contents .= $line;
                if ('while(1)' == trim($line)) {
                    break;
                }
            }

            $new_file_contents .= reduce_pause_calls($file);

            fclose($file);
            
            if (file_put_contents("{$filename}.c", $new_file_contents) === false) {
                throw new UploadException('transformacji', 'Nie możba zmienić zawartości pliku.');
            }
        } else {
            throw new UploadException('transformacji', 'Nie udało otworzyć pliku.');
        } 
    }

    function reduce_pause_calls(&$file) {
        $content = '';
        $pause_count = 0;
        while (($line = fgets($file)) !== false) {
            $line = trim($line);
            if (strpos($line, 'play_pause') === 0) {
                $start = strpos($line, '(') + 1;
                $end  = strpos($line, ')');
                $value = substr($line, $start, $end - $start);
                $pause_count += intval($value);

                if ($pause_count > 256) {
                    $pause_count -= 256;
                    $content .= "\n play_pause(256);";
                }
            } else {
                if ($line != ''){
                    if ($pause_count > 0) {
                        $content .= "\n play_pause({$pause_count});";
                        $pause_count = 0;
                    }

                    $content .= "\n {$line}";
                }
            }
        }

        return $content;
    }

    function compileFile($filename) {
        $out = shell_exec("avr-gcc -std=c99 -Wall -g -Os -mmcu=attiny13 -o {$filename}.bin {$filename}.c 2>&1");
        if($out) {
            throw new UploadException('kompilacji', $out);
        }
    }


    function createHex($filename) {
        $out = shell_exec("avr-objcopy -j .text -j .data -O ihex {$filename}.bin {$filename}.hex 2>&1");
        if($out) {
            throw new UploadException('zmiany pliku na hex', $out);
        }
    }

    function flash($filename) {
        $semaphore = sem_get(AVRDUDE_SEMAPHORE);

        if ($semaphore === false || sem_acquire($semaphore, true) === false) {
            throw new UploadException(
                'wgrywania danych na mikrokontroler',
                'Ktoś już aktualnie próbuje wgrać plik na mikrokontroler.<br>Spróbuj ponownie później.'
            );
        }

        $error_code = shell_exec("sudo -S avrdude -p attiny13 -c usbasp -U flash:w:{$filename}.hex:i -P usb 2>/tmp/avrdude_out; echo $?;");

        sem_release($semaphore);

        if($error_code != 0) {
            throw new UploadException(
                'wgrywania danych na mikrokontroler', 
                file_get_contents('/tmp/avrdude_out')
            );
        }
    }

    function clear($filename) {
        $out = shell_exec("rm avrdude_out {$filename}* 2>/dev/null");
        if($out) {
            throw new UploadException('czyszczenia zbędnych plików', $out);
        }
    }
}
?>
