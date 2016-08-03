/*
 * Chain.cpp
 *
 *  Created on: 2016. 8. 4.
 *      Author: root
 */

#include <stdio.h>

class Chain1 {
public:
	void dodo() {
		printf("[%s] m_chain = %p\n", m_name, m_chain);
		if (m_chain) {
			m_chain->dodo();
		}
	}
	Chain1(const char* name, Chain1* chain) :
	m_name(name),
	m_chain(chain)
	{ }
	virtual ~Chain1() {}
private:
	Chain1* m_chain;
	const char* m_name;
};

class Chain2 : public Chain1 {
public:
	Chain2(const char* name, Chain1* chain) : Chain1(name, chain) {}
};

class Chain3 : public Chain1 {
public:
	Chain3(const char* name, Chain1* chain) : Chain1(name, chain) {}
};

class Chain4 : public Chain1 {
public:
	Chain4(const char* name, Chain1* chain) : Chain1(name, chain) {}
};

int main() {
	Chain1 *chain1 = new Chain1("1", NULL);
	Chain1 *chain2 = new Chain1("2", chain1);
	Chain1 *chain3 = new Chain1("3", chain2);
	chain3->dodo();
}


