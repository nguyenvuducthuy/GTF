//
//  CalcWindow.cpp
//  gtf
//
//  Created by David Gallardo on 31/05/16.
//  Copyright (c) 2016 GTF Development Group. All rights reserved.
//

#include "RigWindow.h"
#include "RigNodes.h"
#include <imgui.h>

CalcWindow::CalcWindow(const char* title) : Window(title, 1040, 720)
{
    calcGraphType = new gtf::NodeGraphType(GTF_UID("Rig"));
    
	auto numberDesc = new gtf::NodeConnectionDesc<gtf::NodeConnectionI32>(GTF_UID("NumberConnection"), "Value");
	auto inputADesc = new gtf::NodeConnectionDesc<gtf::NodeConnectionI32>(GTF_UID("NumberInputConnectionA"), "InputA");
	auto inputBDesc = new gtf::NodeConnectionDesc<gtf::NodeConnectionI32>(GTF_UID("NumberInputConnectionB"), "InputB");

    gtf::NodeTypeBase* nodeType = nullptr;
    nodeType = new gtf::NodeType<CalcNumberNode>(GTF_UID("CalcNumberNode"), "Number");
    nodeType->outputConnectionsDesc.push_back(numberDesc);
    calcGraphType->registerNodeType(nodeType);
    
    nodeType = new gtf::NodeType<CalcAddNode>(GTF_UID("CalcAddNode"), "Add");
    nodeType->inputConnectionsDesc.push_back(inputADesc);
    nodeType->inputConnectionsDesc.push_back(inputBDesc);
    nodeType->outputConnectionsDesc.push_back(numberDesc);
    calcGraphType->registerNodeType(nodeType);
    
    nodeType = new gtf::NodeType<CalcSubstractNode>(GTF_UID("CalcSubstractNode"), "Substract");
    nodeType->inputConnectionsDesc.push_back(inputADesc);
    nodeType->inputConnectionsDesc.push_back(inputBDesc);
    nodeType->outputConnectionsDesc.push_back(numberDesc);
    calcGraphType->registerNodeType(nodeType);
    
    nodeType = new gtf::NodeType<CalcMultiplyNode>(GTF_UID("CalcMultiplyNode"), "Multiply");
    nodeType->inputConnectionsDesc.push_back(inputADesc);
    nodeType->inputConnectionsDesc.push_back(inputBDesc);
    nodeType->outputConnectionsDesc.push_back(numberDesc);
    calcGraphType->registerNodeType(nodeType);
    
    nodeType = new gtf::NodeType<CalcDivideNode>(GTF_UID("CalcDivideNode"), "Divide");
    nodeType->inputConnectionsDesc.push_back(inputADesc);
    nodeType->inputConnectionsDesc.push_back(inputBDesc);
    nodeType->outputConnectionsDesc.push_back(numberDesc);
    calcGraphType->registerNodeType(nodeType);

	auto numberDesc_ = new gtf::NodeConnectionDesc<gtf::NodeConnectionStr>(GTF_UID("NumberConnection"), "RigValue");
	auto numberDesc1_ = new gtf::NodeConnectionDesc<gtf::NodeConnectionStr>(GTF_UID("NumberConnection"), "RigValue1");
	auto inputADesc_ = new gtf::NodeConnectionDesc<gtf::NodeConnectionStr>(GTF_UID("NumberInputConnectionA"), "InputA");
	auto inputBDesc_ = new gtf::NodeConnectionDesc<gtf::NodeConnectionStr>(GTF_UID("NumberInputConnectionB"), "InputB");

	nodeType = new gtf::NodeType<RigAddNode>(GTF_UID("RigAddNode"), "RigAddNode");
	nodeType->inputConnectionsDesc.push_back(inputADesc_);
	nodeType->inputConnectionsDesc.push_back(inputBDesc_);
	nodeType->outputConnectionsDesc.push_back(numberDesc_);
	calcGraphType->registerNodeType(nodeType);

	nodeType = new gtf::NodeType<ArmNode>(GTF_UID("ArmNode"), "arm");
	nodeType->inputConnectionsDesc.push_back(inputADesc_);
	nodeType->outputConnectionsDesc.push_back(numberDesc_);
	calcGraphType->registerNodeType(nodeType);

	nodeType = new gtf::NodeType<TCPNode>(GTF_UID("TCPNode"), "TCPNode");
	nodeType->inputConnectionsDesc.push_back(inputADesc_);
	calcGraphType->registerNodeType(nodeType);

    calcGraphInstance = new CalcNodeGraph(calcGraphType);
    
	gtf::GRHI->setClearColor(0.3f, 0.3f, 0.28f, 1.0f);
}

void CalcWindow::frame(double deltaTime)
{
    calcGraphInstance->updateNodes();
    
    ImGuiWindowFlags flags = 0;
    flags |= ImGuiWindowFlags_NoCollapse;
    flags |= ImGuiWindowFlags_NoMove;
    flags |= ImGuiWindowFlags_NoResize;
    flags |= ImGuiWindowFlags_NoTitleBar;
    flags |= ImGuiWindowFlags_HorizontalScrollbar;
    
    
    float propertiesWith = 250;
    
    ImGui::SetNextWindowPos(ImVec2(m_windowWidth - propertiesWith - 20.0f, 20.0f), ImGuiSetCond_Always);
    ImGui::SetNextWindowSize(ImVec2(propertiesWith, m_windowHeight - 240.0f), ImGuiSetCond_Always);
    
    ImGui::Begin("NodeProperties", nullptr, flags	);
    {
        ImGui::Text("Node Properties");
        ImGui::Separator();
        
        if(calcGraphInstance->selectedNodes.size() == 1)
        {
            CalcNode* node = CalcNode::CAST(calcGraphInstance->selectedNodes.front());
            if(node && node->type == ECalcNodeType::CNT_NUMBER)
            {
                node->dirty = ImGui::InputInt("Value", &node->number);
            }

			RigNode* node_ = RigNode::CAST(calcGraphInstance->selectedNodes.front());
			if(node_ && node_->type == ECalcNodeType::CNT_RIGCOMP)
			{
				char * nodeVal = (char*)node_->result.c_str();
				node_->dirty = ImGui::InputTextMultiline("Value", nodeVal, 66666);
			}
            
        }
    }
    ImGui::End();
    
    ImGui::SetNextWindowPos(ImVec2(m_windowWidth - propertiesWith - 20.0f, m_windowHeight - 200.0f), ImGuiSetCond_Always);
    ImGui::SetNextWindowSize(ImVec2(propertiesWith, 180.0f), ImGuiSetCond_Always);
    
    if(ImGui::Begin("Controls", nullptr, flags))
    {
        ImGui::Text("Controls");
        ImGui::Separator(); ImGui::Spacing();
        
        ImGui::Text("Right-Click to open menu."); ImGui::Spacing();
        ImGui::Text("Drag connections to link."); ImGui::Spacing();
        ImGui::Text("[Shit]+Click on a node for multi \nselection."); ImGui::Spacing();
        ImGui::Text("[Ctrl]+Click on a connection \nto break links."); ImGui::Spacing();
        ImGui::Text("[Supr] to delete selected nodes"); ImGui::Spacing();
        
    }
    ImGui::End();
    
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiSetCond_Always);
    ImGui::SetNextWindowSize(ImVec2(m_windowWidth - propertiesWith - 60.0f, m_windowHeight - 40.0f), ImGuiSetCond_Always);
    
    if(ImGui::Begin("Graph", nullptr, flags))
    {
        calcGraphInstance->updateGUI();
    }
    ImGui::End();
    
    
    
    // Rendering
    gtf::GRHI->viewport(0, 0, m_windowWidth, m_windowHeight);
	gtf::GRHI->clearColorAndDepthBuffers();
}
