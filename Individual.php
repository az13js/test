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
    /** @var array DNA 编码 */
    private $dna = [];

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
     *
     * @param EnvironmentInfo $environmentInfo 活动的时候先接收周围环境的信息
     */
    public function action($environmentInfo)
    {
        echo 'Hello, world!' . PHP_EOL;
        echo 'Oh, environment info:' . PHP_EOL;
        var_dump($environmentInfo);
    }

    /**
     * 初始化 DNA 数据
     */
    private function initDna()
    {
        for ($i = 0; $i < 10; $i++) {
            $this->dna[] = mt_rand(0, 9);
        }
    }
}
