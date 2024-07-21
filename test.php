<?php
class MTRandom3
{
	public $native;
	public function __construct($seed = null){
		$this->native = new \Random\Randomizer(new \Random\Engine\Mt19937($seed));
	}
	public function setSeed($seed){
		$this->native = new \Random\Randomizer(new \Random\Engine\Mt19937($seed)); 
	}
	public function genRandInt(){
		return $this->native->getInt(0, 0x7fffffff);
	}
	public function nextInt($bound = null){
		return $bound == null ? $this->native->nextInt() : $this->native->getInt(0, $bound - 1);
	}
	public function nextFloat(){
		return $this->native->getInt(0, 0x7fffffff) / 0x7fffffff;
	}
}


/**
 * Fatser implementation of Mersenne Twister
 */
class MTRandom2
{
	const MAG = [0, 0x9908b0df];
	public $mt;
	public $index = 0;
	public function __construct($seed = null){
		$this->setSeed($seed);
	}
	
	public function setSeed($seed){
		$this->mt[0] = $seed & 0xffffffff;
		for($this->index = 1; $this->index < 624; ++$this->index){
			$this->mt[$this->index] = (0x6c078965 * ($this->mt[$this->index-1] >> 30 ^ $this->mt[$this->index - 1]) + $this->index) & 0xffffffff;
		}
	}

	public function genRandInt(){
		if($this->index >= 624 || $this->index < 0){
			if($this->index >= 625 || $this->index < 0) $this->setSeed(4357);
			
			for($kk = 0; $kk < 227; ++$kk){
				$y = ($this->mt[$kk] & 0x80000000) | ($this->mt[$kk + 1] & 0x7fffffff);
				$this->mt[$kk] = $this->mt[$kk+397] ^ ($y >> 1) ^ MTRandom2::MAG[$y & 0x1];
			}
			
			for(;$kk < 623; ++$kk){
				$y = ($this->mt[$kk] & 0x80000000) | ($this->mt[$kk + 1] & 0x7fffffff);
				$this->mt[$kk] = $this->mt[$kk-227] ^ ($y >> 1) ^ MTRandom2::MAG[$y & 0x1];
			}
			
			$y = ($this->mt[623] & 0x80000000) | ($this->mt[0] & 0x7fffffff);
			$this->mt[623] = $this->mt[396] ^ ($y >> 1) ^ MTRandom2::MAG[$y & 0x1];
			$this->index = 0;
		}
		
		$y = $this->mt[$this->index++];
		$y ^= ($y >> 11);
		$y ^= ($y << 7) & 0x9d2c5680;
		$y ^= ($y << 15) & 0xefc60000;
		$y ^= ($y >> 18);
		return $y;
	}
	
	public function nextInt($bound = null){
		return $bound == null ? $this->genRandInt() >> 1 : $this->genRandInt() % $bound;
	}
	
	public function nextFloat(){
		return $this->genRandInt() / 0xffffffff;
	}
}

$mt3 = new \Random\Randomizer(new \Random\Engine\Mt19937(1));
$mt2 = new MTRandom2(1);
$mtn = new MTRandom(1);
echo $mt2->nextInt() . ":" . $mtn->nextInt()."\n";
echo $mt2->nextInt(5555) . ":" . $mtn->nextInt(5555)."\n";
echo $mt2->nextFloat().":".$mtn->nextFloat().":".($mt3->getInt(0, 0x7fffffff) / 0x7fffffff)."\n";

for($i = 0; $i < 700; ++$i){
	$mt2->nextFloat();
	$mtn->nextFloat();
}

echo $mt2->nextFloat()."\n";
echo $mtn->nextFloat()."\n";


const N = 10000000;
$t = microtime(1);
for($i = 0; $i < N; ++$i){
	$mt2->nextFloat();
}
$t = microtime(1) - $t;
echo "mt2 $t\n";

$t = microtime(1);
for($i = 0; $i < N; ++$i){
	$mt3->getInt(0, 0x7fffffff) / 0x7fffffff;
}
$t = microtime(1) - $t;
echo "mt3 $t\n";


$t = microtime(1);
for($i = 0; $i < N; ++$i){
	$mtn->nextFloat();
}
$t = microtime(1) - $t;
echo "mtn $t\n";
