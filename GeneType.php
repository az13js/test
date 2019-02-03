<?php
class GeneType
{
    private $geneTypeName;

    /**
     * create a genetype:
     * $myType1 = new GeneType('A');
     * $myType2 = new GeneType('B');
     *
     * @param string $geneType
     */
    public function __construct($geneTypeName)
    {
        $this->geneTypeName = $geneTypeName;
    }

    /**
     * get the genetype's name
     * var_dump($myType1->typeName());
     *
     * @return string
     */
    public function typeName()
    {
        return $this->geneTypeName;
    }
}
