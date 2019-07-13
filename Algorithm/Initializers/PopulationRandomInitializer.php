<?php
namespace Algorithm\Initializers;

use Algorithm\Factories\GeneFactory;
use Algorithm\Individual;
use Algorithm\Population;

/**
 * @author az13js
 */
class PopulationRandomInitializer
{
    /** @var array 基因类型数组 */
    private $types = [];
    /** @var int 个体基因长度 */
    private $geneLength = 0;
    /** @var int 种群大小 */
    private $size = 0;

    /**
     * @param array $types 字符串的数组，基因类型
     * @param int $len 基因长度
     * @param int $size 种群的大小
     */
    public function __construct(array $types, int $len, int $size)
    {
        $this->types = $types;
        $this->geneLength = $len;
        $this->size = $size;
    }

    /**
     * 创建种群
     *
     * @return object 种群对象
     */
    public function create()
    {
        $geneFactory = new GeneFactory();
        $population = new Population();
        for ($i = 0; $i < $this->size; $i++) {
            $population->addIndividual(new Individual($geneFactory->create($this->types, $this->geneLength)));
        }
        return $population;
    }
}
