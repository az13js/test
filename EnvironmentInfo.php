<?php
/**
 * 传递给生物的环境的信息
 */
class EnvironmentInfo
{
    private $individuals = [];

    public function add($individual)
    {
        $this->individuals[] = $individual;
    }
}
