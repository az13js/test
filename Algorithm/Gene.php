<?php
namespace Algorithm;

/**
 * 基因
 *
 * @author az13js
 */
class Gene extends AbstractGene
{
    /** @var string 保存类型，一般是字符串 */
    private $type;

    /**
     * @param string $type
     */
    public function __construct(string $type)
    {
        $this->type = $type;
    }

    /**
     * 获取基因的类型名称
     *
     * @return string
     */
    public function getType()
    {
        return $this->type;
    }
}
