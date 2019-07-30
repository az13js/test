<?php
require_once 'autoload.php';

$f = new CellLab\NeuronFactory();
$p = $f->createNeuron();
$p->run();
