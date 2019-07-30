<?php
namespace CellLab\Neuron;

class NeuronFactory implements NeuronFactoryInterface
{
    public function createNeuron()
    {
        return new Neuron();
    }
}
