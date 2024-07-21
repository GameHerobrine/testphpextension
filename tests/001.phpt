--TEST--
Check if nativenc is loaded
--SKIPIF--
<?php
if (!extension_loaded('nativenc')) {
    echo 'skip';
}
?>
--FILE--
<?php
echo 'The extension "nativenc" is available';
?>
--EXPECT--
The extension "nativenc" is available
