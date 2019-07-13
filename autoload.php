<?php
/**
 * 用来自动加载类文件
 *
 * 类的命名空间结构跟文件位置对应。
 */
spl_autoload_register(function($class) {
    $ds = DIRECTORY_SEPARATOR;
    $namespacedir = str_replace('\\', $ds, $class . '.php');
    $file = __DIR__ . $ds . $namespacedir;
    require_once $file;
});
