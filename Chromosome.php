<?php
class Chromosome
{
    private $geneChain = [];

    /**
     * append a geneType in a chromosome.
     *
     * $ch = new Chromosome();
     * $ch->append(new GeneType('A'));
     *
     * @param GeneType $geneType
     * @return void
     */
    public function append($geneType)
    {
        $this->geneChain[] = $geneType;
    }
}
