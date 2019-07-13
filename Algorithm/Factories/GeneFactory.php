<?php
namespace Algorithm\Factories;

use Algorithm\Gene;

/**
 * @author az13js
 */
class GeneFactory
{
    /**
     * @param string|array 指定创建的基因类型或字符串数组方式随机生成
     * @param int $num 默认生成1个，直接返回，大于1时返回数组
     * @return object|array 一个或多个基因
     */
    public function create($type, int $num = 1)
    {
        if (1 == $num) {
            return $this->createGene($type);
        } else {
            $genes = [];
            for ($i = 0; $i < $num; $i++) {
                $genes[] = $this->createGene($type);
            }
            return $genes;
        }
    }

    /**
     * @param array|string 基因型
     * @return object 基因实例
     */
    private function createGene($type)
    {
        if (is_array($type)) {
            return new Gene($type[mt_rand(0, count($type) - 1)]);
        } else {
            return new Gene($type);
        }
    }
}
