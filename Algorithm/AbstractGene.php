<?php
namespace Algorithm;

/**
 * 基因
 *
 * 遗传算法里面的基因实际上是编码个体的最基本单位。它有不同但是数量有限的类型。
 *
 * @author az13js
 */
abstract class AbstractGene
{
    /**
     * @param string $type
     */
    abstract public function __construct(string $type);

    /**
     * 获取基因的类型名称
     *
     * @return string
     */
    abstract public function getType();
}
