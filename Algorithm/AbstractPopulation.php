<?php
namespace Algorithm;

/**
 * 种群
 *
 * 种群由多个个体组成，这些个体没有顺序
 *
 * @author az13js
 */
abstract class AbstractPopulation
{
    /**
     * @param array 种群中的个体，可以不用传
     */
    abstract public function __construct(array $individuals = []);

    /**
     * 给种群添加一个个体对象，不管这个个体是不是已经存在。
     *
     * @param object $individual 个体实例
     * @return void
     */
    abstract public function addIndividual($individual);
}
