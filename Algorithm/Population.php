<?php
namespace Algorithm;

/**
 * 种群
 *
 * 种群由多个个体组成，这些个体没有顺序
 *
 * @author az13js
 */
class Population extends AbstractPopulation
{
    /** @var array 保存种群中的个体对象 */
    private $individuals = [];

    /**
     * @param array 种群中的个体，可以不用传
     */
    public function __construct(array $individuals = [])
    {
        $this->individuals = $individuals;
    }

    /**
     * 给种群添加一个个体对象，不管这个个体是不是已经存在。
     *
     * @param object $individual 个体实例
     * @return void
     */
    public function addIndividual($individual)
    {
        $this->individuals[] = $individual;
    }
}
