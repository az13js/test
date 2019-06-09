<?php
/**
 * 有生命的个体
 */
class Individual
{
    /** @var int 在地球上的x坐标 */
    private $x;
    /** @var int 在地球上的y坐标 */
    private $y;
    /** @var int 生物的类型id */
    private $typeId;

    /**
     * 生命诞生时，会被赋予一个坐标，和生命的类型
     *
     * @param int $x
     * @param int $y
     * @param int $typeId
     */
    public function __construct(int $x, int $y, $typeId = 0)
    {
        $this->x = $x;
        $this->y = $y;
        $this->typeId = $typeId;
    }

    /**
     * 生命随时都有活动，比如说句话
     */
    public function action()
    {
        echo 'Hello, world!' . PHP_EOL;
    }
}
