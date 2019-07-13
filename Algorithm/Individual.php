<?php
namespace Algorithm;

/**
 * 个体
 *
 * 个体是由有序序列的基因组成的，也就是基因对象的序列。
 *
 * @author az13js
 */
class Individual extends AbstractIndividual
{
    /** @var array 基因对象的有序序列 */
    private $sequence = [];

    /**
     * @param array 基因对象的数组
     */
    public function __construct(array $genes)
    {
        $this->sequence = $genes;
    }
}
