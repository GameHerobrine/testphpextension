<?php

//$arr = new Bytearray(16);
//$arr->fill(2);
//echo $arr[false]."\n";
//unset($arr);

$arr = array_fill(0, 256, 0);

$narr = new Bytearray(256);
for($i = 0; $i < 256; ++$i) $narr->fill(0);

$s = str_repeat("\x00", 256);

const N = 10000000;


$t = microtime(1);
for($i = 0; $i < N; ++$i){
	$arr[$i & 0xff] = $i & 0xff;
}
$t = microtime(1) - $t;
echo "Arr set: $t\n";

$t = microtime(1);
for($i = 0; $i < N; ++$i){
	$m = $arr[$i & 0xff];
}
$t = microtime(1) - $t;
echo "Arr get: $t\n";

//$t = microtime(1);
//for($i = 0; $i < ; ++$i){
//	$narr[$i & 0xff] = $i & 0xff;
//}
//$t = microtime(1) - $t;
//echo "Narr set: $t\n";

$t = microtime(1);
for($i = 0; $i < N; ++$i){
	$m = $narr[$i & 0xff];
}
$t = microtime(1) - $t;
echo "Narr get: $t\n";
