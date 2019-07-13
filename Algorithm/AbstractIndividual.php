<?php
namespace Algorithm;

/**
 * 个体
 *
 * 个体是由有序序列的基因组成的，也就是基因对象的序列。
 *
 * @author az13js
 */
abstract class AbstractIndividual
{
    /**
     * @param array 基因对象的数组
     */
    abstract public function __construct(array $genes);
}
