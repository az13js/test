<?php
require_once 'autoload.php';
var_dump(memory_get_peak_usage());
$x = new Algorithm\Initializers\PopulationRandomInitializer(['0','1'], 3000, 200);
$x = $x->create();
var_dump(gettype($x));
file_put_contents('x.txt', serialize($x));
var_dump(memory_get_peak_usage());
