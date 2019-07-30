<?php
namespace CellLab;

class NeuronFactory implements NeuronFactoryInterface
{
    public function createNeuron()
    {
        return new Neuron();
    }
}
