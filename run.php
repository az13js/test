<?php
require_once 'autoload.php';

$f = new CellLab\Neuron\NeuronFactory();
$p = $f->createNeuron();
$p->run();
