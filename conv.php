<?php
$fileNameAndContents = [];

$d = opendir(__DIR__);
if (false === $d) {
    echo '打开目录失败' . PHP_EOL;
    exit(1);
}

while (false !== ($file = readdir($d))) {
    if (in_array($file, ['.', '..'])) {
        continue;
    }
    if (!is_file($file)) {
        continue;
    }
    $ext = pathinfo($file, PATHINFO_EXTENSION);
    if (!in_array($ext, ['cpp', 'h', 'php', 'txt', 'sh', 'service']) && $file != 'Makefile') {
        continue;
    }
    $contents = file_get_contents($file);
    $contents = str_replace("\r\n", "\n", $contents);
    $fileNameAndContents[$file] = $contents;
}

closedir($d);

foreach ($fileNameAndContents as $file => $contents) {
    echo $file . PHP_EOL;
    file_put_contents($file, $contents);
}
