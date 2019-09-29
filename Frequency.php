<?php
$data = [];
for ($i = 0; $i < 100; $i++) {
    $sample = [];
    if (mt_rand() / mt_getrandmax() >= 0.5) {
        $sample[] = 'A';
    }
    $data[] = $sample;
}

$event = ['A'];