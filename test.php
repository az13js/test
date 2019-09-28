<?php
function createNormalCurlHandle(string $address)
{
    $handle = curl_init();
    if (false === $handle) {
        return false;
    }
    $setResult = curl_setopt_array($handle, [
        CURLOPT_AUTOREFERER => true,
        CURLOPT_COOKIESESSION => false,
        CURLOPT_FILETIME => true,
        CURLOPT_FOLLOWLOCATION => true,
        CURLOPT_FORBID_REUSE => false,
        CURLOPT_FRESH_CONNECT => false,
        CURLOPT_HTTPPROXYTUNNEL => false,
        CURLOPT_NETRC => false,
        CURLOPT_RETURNTRANSFER => true,
        CURLOPT_UNRESTRICTED_AUTH => false,
        CURLOPT_UPLOAD => false,
        CURLOPT_VERBOSE => false,
        CURLOPT_CONNECTTIMEOUT => 30,
        CURLOPT_TIMEOUT => 60,
        CURLOPT_DNS_CACHE_TIMEOUT => 1000,
        CURLOPT_MAXCONNECTS => 65535,
        CURLOPT_MAXREDIRS => 20,
        /*CURLOPT_COOKIE => '',
        CURLOPT_REFERER => '',
        CURLOPT_USERAGENT => '',
        CURLOPT_HTTPHEADER => [],
        */
        CURLOPT_URL => $address,
    ]);
    if (false === $setResult) {
        curl_close($handle);
        return false;
    }
    return $handle;
}

echo date('Y-m-d H:i:s') . ' [INFO] 开始执行' . PHP_EOL;

$urls = [
    'https://www.baidu.com',
    'https://www.china.com',
    'https://cn.bing.com',
    'https://cn.bing.com',
    'https://cn.bing.com',
    'https://cn.bing.com',
    'https://cn.bing.com',
    'https://cn.bing.com',
    'https://cn.bing.com',
    'https://cn.bing.com',
    'https://cn.bing.com',
    'https://www.google.com',
    'https://www.google.com',
];

$total = count($urls);

$handle = curl_multi_init();
if (false === $handle) {
    echo date('Y-m-d H:i:s') . ' [ERROR] 初始化失败' . PHP_EOL;
    die();
}
curl_multi_setopt($handle, CURLMOPT_PIPELINING, 1);
curl_multi_setopt($handle, CURLMOPT_MAXCONNECTS, 65535);

$normalHandles = [];
foreach ($urls as $address) {
    echo date('Y-m-d H:i:s') . ' [INFO] 添加请求 ' . $address . PHP_EOL;
    $normalHandle = createNormalCurlHandle($address);
    if (false === $normalHandle) {
        echo date('Y-m-d H:i:s') . ' [ERROR] 创建资源失败' . PHP_EOL;
        die();
    } else {
        echo date('Y-m-d H:i:s') . ' [INFO] 资源创建成功' . PHP_EOL;
    }
    $normalHandles[] = $normalHandle;

    echo date('Y-m-d H:i:s') . ' [INFO] 添加到并发请求资源内' . PHP_EOL;
    $addResult = curl_multi_add_handle($handle, $normalHandle);
    if (0 !== $addResult) {
        echo date('Y-m-d H:i:s') . ' [ERROR] 创建资源失败，错误内容 ' . $addResult . PHP_EOL;
        die();
    } else {
        echo date('Y-m-d H:i:s') . ' [INFO] 添加成功' . PHP_EOL;
    }
}

echo date('Y-m-d H:i:s') . ' [INFO] 执行请求' . PHP_EOL;
$running = 0;
while (true) {
    curl_multi_exec($handle, $running);
    if ($running > 0) {
        curl_multi_select($handle, 60);
    } else {
        break;
    }
}

echo date('Y-m-d H:i:s') . ' [INFO] 执行结果' . PHP_EOL;
foreach ($normalHandles as $i => $normalHandle) {
    $content = curl_multi_getcontent($normalHandle);
    curl_close($normalHandle);
    echo date('Y-m-d H:i:s') . ' [INFO] 链接 ' . $urls[$i] . PHP_EOL;
    echo date('Y-m-d H:i:s') . ' [INFO] 返回数据大小 ' . strlen($content) . PHP_EOL;
}

curl_multi_close($handle);
echo date('Y-m-d H:i:s') . ' [INFO] 结束' . PHP_EOL;
