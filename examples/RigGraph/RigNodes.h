//
//  CalcNodes.h
//  gtf
//
//  Created by David Gallardo on 31/05/16.
//  Copyright (c) 2016 GTF Development Group. All rights reserved.
//

#pragma once

#include <gtf/NodeGraph.h>
#include <cstdint>
#include <list>
#include <vector>
#include <winsock2.h>
//#include "TCPServer.h"

#define MAX_CONNECTION_COUNT 32

enum class ECalcConnectionType : uint8_t
{
    CCT_INT32
};

enum class ECalcNodeType : uint8_t
{
    CNT_UNDEFINED,
    CNT_NUMBER,
    CNT_ADD,
    CNT_SUBSTRACT,
    CNT_MULTIPLY,
    CNT_DIVIDE,
    CNT_RIG,
    CNT_ARM,
    CNT_TCP
};

struct ConnectionDesc
{
	const char* name;
	ECalcConnectionType type;
};


class CalcNode : public gtf::Node
{
public:
    CalcNode(ECalcNodeType _t) : type(_t){};
    
    bool dirty { true };
    ECalcNodeType type;
    int number { 0 };
    
    static CalcNode* CAST(gtf::Node* node){ return dynamic_cast<CalcNode*>(node); }
};

class CalcNumberNode : public CalcNode
{
public:
    CalcNumberNode(ECalcNodeType _t) : CalcNode(_t){};
    CalcNumberNode() : CalcNode(ECalcNodeType::CNT_NUMBER)
    {
        number = rand() % 60;
    };
    
    void update() override;
    
};

class CalcMathOpNode : public CalcNumberNode
{
public:
    CalcMathOpNode(ECalcNodeType _t) : CalcNumberNode(_t){};
    void update() override;
    
    virtual int MathOp(int a, int b){ return 0; }
};

class CalcAddNode : public CalcMathOpNode
{
public:
    CalcAddNode() : CalcMathOpNode(ECalcNodeType::CNT_ADD){};
    int MathOp(int a, int b) override { return a + b; }
};

class CalcSubstractNode : public CalcMathOpNode
{
public:
    CalcSubstractNode() : CalcMathOpNode(ECalcNodeType::CNT_SUBSTRACT){};
    int MathOp(int a, int b){ return a - b; }
};

class CalcMultiplyNode : public CalcMathOpNode
{
public:
    CalcMultiplyNode() : CalcMathOpNode(ECalcNodeType::CNT_MULTIPLY){};
    int MathOp(int a, int b){ return a * b; }
};

class CalcDivideNode : public CalcMathOpNode
{
public:
    CalcDivideNode() : CalcMathOpNode(ECalcNodeType::CNT_DIVIDE){};
    int MathOp(int a, int b){ return (b == 0) ? 0 : a / b; }
};

//--------------------------------------------------------

class RigNode : public gtf::Node
{
public:
	RigNode(ECalcNodeType _t) : type(_t) {};

	bool dirty{ true };
	ECalcNodeType type;
	std::string result{ "none" };

	static RigNode* CAST(gtf::Node* node) { return dynamic_cast<RigNode*>(node); }
};

class ComponentNode : public RigNode
{
public:
	ComponentNode(ECalcNodeType _t) : RigNode(_t) {};
	ComponentNode() : RigNode(ECalcNodeType::CNT_RIG) {};

	void update() override;

};

class RigOpNode : public ComponentNode
{
public:
	RigOpNode(ECalcNodeType _t) : ComponentNode(_t) {};
	void update() override;

	virtual std::string RigOp(std::string a, std::string b) { return ""; }
};

class ArmNode : public ComponentNode
{
public:
	ArmNode() : ComponentNode(ECalcNodeType::CNT_RIG)
	{
		result = "arm";
	};
};

class RigAddNode : public RigOpNode
{
public:
	RigAddNode() : RigOpNode(ECalcNodeType::CNT_ARM) {};
	std::string RigOp(std::string a, std::string b) { return a + b; }
};

class TCPNode : public gtf::Node
{
public:
	TCPNode();
	int createTCP(char* message);
	bool dirty{ true };
	std::string msg;
	//TcpServer * thuy;
	SOCKET ListenSocket;
	SOCKET ClientSocket;

	void update() override;

	static TCPNode* CAST(gtf::Node* node) { return dynamic_cast<TCPNode*>(node); }
};

