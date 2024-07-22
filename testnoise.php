<?php
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


class NoiseGeneratorPerlin2{
	public $octaves, $offsetX, $offsetY, $offsetZ, $perm;
	public static $grad3 = [
		[1, 1, 0], [-1, 1, 0], [1, -1, 0], [-1, -1, 0],
		[1, 0, 1], [-1, 0, 1], [1, 0, -1], [-1, 0, -1],
		[0, 1, 1], [0, -1, 1], [0, 1, -1], [0, -1, -1]
	];
	
	public static function lerp($x, $y, $z){
		return $y + $x * ($z - $y);
	}
	
	public static function grad($hash, $x, $y, $z){
		$hash &= 15;
		$u = $hash < 8 ? $x : $y;
		$v = $hash < 4 ? $y : (($hash === 12 or $hash === 14) ? $x : $z);
		
		return (($hash & 1) === 0 ? $u : -$u) + (($hash & 2) === 0 ? $v : -$v);
	}
	
	public function __construct($random, $octaves){
		$this->octaves = $octaves;
		$this->offsetX = $random->nextFloat() * 256;
		$this->offsetY = $random->nextFloat() * 256;
		$this->offsetZ = $random->nextFloat() * 256;
		
		for($i = 0; $i < 512; ++$i){
			$this->perm[$i] = 0;
		}
		
		for($i = 0; $i < 256; ++$i){
			$this->perm[$i] = $random->nextInt(256);
		}
		
		for($i = 0; $i < 256; ++$i){
			$pos = $random->nextInt(256 - $i) + $i;
			$old = $this->perm[$i];
			
			$this->perm[$i] = $this->perm[$pos];
			$this->perm[$pos] = $old;
			$this->perm[$i + 256] = $this->perm[$i];
		}
	}
	
	public function getNoise3D($x, $y, $z){
		$x += $this->offsetX;
		$y += $this->offsetY;
		$z += $this->offsetZ;
	
		$floorX = floor($x); //normal floor is faster
		$floorY = floor($y);
		$floorZ = floor($z);
		
		$X = $floorX & 0xFF;
		$Y = $floorY & 0xFF;
		$Z = $floorZ & 0xFF;
		
		$x -= $floorX;
		$y -= $floorY;
		$z -= $floorZ;
		
		//Fade curves
		$fX = $x * $x * $x * ($x * ($x * 6 - 15) + 10);
		$fY = $y * $y * $y * ($y * ($y * 6 - 15) + 10);
		$fZ = $z * $z * $z * ($z * ($z * 6 - 15) + 10);
		
		//Cube corners
		$A = $this->perm[$X] + $Y;
		$AA = $this->perm[$A] + $Z;
		$AB = $this->perm[$A + 1] + $Z;
		
		$B = $this->perm[$X + 1] + $Y;
		$BA = $this->perm[$B] + $Z;
		$BB = $this->perm[$B + 1] + $Z;
		
		$m = self::lerp($fX, self::grad($this->perm[$AA], $x, $y, $z), self::grad($this->perm[$BA], $x - 1, $y, $z));
		$n = self::lerp($fX, self::grad($this->perm[$AB], $x, $y-1, $z), self::grad($this->perm[$BB], $x-1, $y-1, $z));
		$o = self::lerp($fX, self::grad($this->perm[$AA + 1], $x, $y, $z - 1), self::grad($this->perm[$BA + 1], $x - 1, $y, $z - 1));
		$p = self::lerp($fX, self::grad($this->perm[$AB + 1], $x, $y - 1, $z - 1), self::grad($this->perm[$BB + 1], $x - 1, $y - 1, $z - 1));
		
		return self::lerp($fZ, 
			self::lerp($fY, 
				$m, 
				$n
			), 
			self::lerp($fY, 
				$o, $p
			
			)
		);
	}
	
	public function getNoise2D($x, $y){
		return $this->getNoise3D($x, $y, 0);
	}
	
	public function noise2D($x, $z, $frequency, $amplitude, $normalized = false){
		$result = 0;
		$amp = 1;
		$freq = 1;
		$max = 0;
		
		for($i = 0; $i < $this->octaves; ++$i){
			$result += $this->getNoise2D($x * $freq, $z * $freq) * $amp;
			$max += $amp;
			$freq *= $frequency;
			$amp *= $amplitude;
		}
		if($normalized === true){
			$result /= $max;
		}
		
		return $result;
	}
	
	public function noise3D($x, $y, $z, $frequency, $amplitude){
		$result = 0;
		$amp = 1;
		$freq = 1;
		$max = 0;
		
		for($i = 0; $i < $this->octaves; ++$i){
			$result += $this->getNoise3D($x * $freq, $y * $freq, $z * $freq) * $amp;
			$max += $amp;
			$freq *= $frequency;
			$amp *= $amplitude;
		}
		$result /= $max;
		
		return $result;
	}
}


$mt = new MTRandom(134);
$n = new NoiseGeneratorPerlin($mt, 16);
$n2 = new NoiseGeneratorPerlin2(new MTRandom2(134), 16);

//echo $n->noise3D(2, 3, 4, 0.5, 0.5);

const N = 100000;

$t = microtime(1);
for($i = 0; $i < N; ++$i){
	$n->noise3D($i, 0.5, $i, 0.5, 0.5);
}
$t = microtime(1) - $t;
echo "n time: $t\n";

$t = microtime(1);
for($i = 0; $i < N; ++$i){
	$n2->noise3D($i, 0.5, $i, 0.5, 0.5);
}
$t = microtime(1) - $t;
echo "n2 time: $t\n";
