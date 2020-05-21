#pragma once

#include<time.h>
#include<stdlib.h>
#include<iostream>
#include<bitset>
using namespace std;

#define random(x) rand()%x

static int p, q, n, e, d;

//使用1000以内的随机质数来简单化实现随机大质数
int primelist[] = { 2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,
53,59,61,67,71,73,79,83,89,97,
101,103,107,109,113,127,131,137,139,149,
151,157,163,167,173,179,181,191,193,197,199,
211,223,227,229,233,239,241,
251,257,263,269,271,277,281,283,293,
307,311,313,317,331,337,347,349,
353,359,367,373,379,383,389,397,
401,409,419,421,431,433,439,443,449,
457,461,463,467,479,487,491,499,
503,509,521,523,541,547,
557,563,569,571,577,587,593,599,
601,607,613,617,619,631,641,643,647,
653,659,661,673,677,683,691,
701,709,719,727,733,739,743,
751,757,761,769,773,787,797,
809,811,821,823,827,829,839,
853,857,859,863,877,881,883,887,
907,911,919,929,937,941,947,
953,967,971,977,983,991,997 };

//随机取两个不同质数赋给p,q
void getPrime() {
	int num = sizeof(primelist) / sizeof(int);
	//cout << "number of prime:" << num << endl;
	do {
		p = primelist[random(num)];
		q = primelist[random(num)];
	} while (p == q);
	n = p*q;
	cout << "p,q=" << p << " " << q << endl;
}

//模乘 (a*b)%n
long long mulmod(int a, int b, int n) {
	long long la = a, lb = b, ln = n;
	return ((la%ln)*(lb%ln)) % ln;
}

//模幂 (a^b)%n
int powmod(int a, int b, int n) {
	long long res = 1;
	while (b) {
		while (!(b & 1)) {
			b >>= 1;
			a = mulmod(a, a, n);
		}
		b--;
		res = mulmod(a, res, n);
	}
	return res;
}

//最大公约数
int gcd(int p, int q) {
	if (p*q == 0) {
		return 0;
	}
	int a = p > q ? p : q;
	int b = p < q ? p : q;
	int t;
	if (p == q) {
		return p;
	}
	else {
		while (b) {
			a = a%b;
			t = a;
			a = b;
			b = t;
		}
		return a;
	}
}

//欧拉函数
int eular(int n) {
	int i,res = n;
	for (i = 2; i <= n; i++) {
		if (n%i == 0) {
			while (n%i == 0) {
				n /= i;
			}
			res = res*(i - 1) / i;
		}
	}
	cout << "fai(n)=" << res << endl;
	return res;
}

//计算e
void gete() {
	int f = eular(n);
	do {
		e = random(f);
	} while (gcd(e, f) != 1);
	cout << "e,n=" << e << " " << n << endl;
}

//计算d
void getd() {
	d = 1;
	int f = eular(n);
	while (1) {
		if (mulmod(d,e,f) == 1) {
			cout << "d,n=" << d << " " << n << endl;
			return;
		}
		d++;
	}
}

//初始化
void RSAinit() {
	getPrime();
	gete();
	getd();
}

//加密
void RSAencrypt(string key,int* cypher) {
	cout << "RSA cypher:";
	for (int i = 0; i < 8; i++) {
		cypher[i] = powmod((int)key[i], e, n);
		cout << cypher[i] << " ";
	}
	cout << endl;
}

//解密
void RSAdecrypt(string &key,int* cypher) {
	for (int i = 0; i < 8; i++) {
		char c = (char)powmod(cypher[i], d, n);
		key += c;
	}
	cout << "RSA plain:" << key << endl;
}