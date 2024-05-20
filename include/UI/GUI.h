#pragma once
#include "imgUI/imgui.h"
#include "UI/TextEditor.h"
#include <vector>

std::vector <int> DragList;

TextEditor* te;
// �����û���ǰλ��
void DrawDebugUI(glm::vec3 position){
    ImGui::Begin("Debug Window");
    ImGui::Text("user position : (%f,%f,%f)", position.x, position.y, position.z);
    ImGui::End();
}

void DrawMenuUI(Camera& camera, float& scale, ImVec4& backgroundColor){
    ImGui::Begin(u8"Menu");
    ImGui::Text("Background Color");
    ImGui::SameLine();
    if (ImGui::Button("Reset"))
    {
        // ���ñ�����ɫ��ֵ
        backgroundColor = ImVec4(0.0f,0.0f,0.0f,1.0f);
    }
    // ʹ�õ�ɫ�������ı�����ɫ
    if (ImGui::ColorPicker4("##UNIQALID", (float*)&backgroundColor,
                            ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_PickerHueBar,
                            NULL))
    {
        // ����û��ڵ�ɫ����ѡ������ɫ�����������ﴦ����ɫ�ı���߼�
    }
//    ImGui::Text(u8"scale");
//    if (ImGui::SliderFloat("##uniqueID", &scale, -1.0f, 1.0f))
//    {
//        // ���û�����������ʱ������Ĵ��뽫��ִ�У�������С
//        camera.ProcessMouseScroll(scale);
//    }
//    ImGui::SameLine();
//    // ����ť������ʱ������scale��ֵ
//    if (ImGui::Button("Reset"))
//    {
//        // ����camera��ֵ
//        camera.ProcessMouseScroll(50);
//        scale = 0.0f;
//    }
    ImGui::Text(u8"position");
    // �����������������camera��λ��
    float cameraPosition[3] = {camera.Position.x, camera.Position.y, camera.Position.z};
    if (ImGui::SliderFloat("X", &cameraPosition[0], -30.0f, 30.0f))
    {
        // ���û��޸Ļ����ֵʱ������camera��λ��
        camera.Position.x = cameraPosition[0];
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset X"))
    {
        camera.Position.x = 0.0f;
    }

    if (ImGui::SliderFloat(u8"Y", &cameraPosition[1], -30.0f, 30.0f))
    {
        camera.Position.y = cameraPosition[1];
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset Y"))
    {
        camera.Position.y = 0.0f;
    }

    if (ImGui::SliderFloat(u8"Z", &cameraPosition[2], -30.0f, 30.0f))
    {
        camera.Position.z = cameraPosition[2];
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset Z"))
    {
        camera.Position.z = 3.0f;
    }
    ImGui::End();
}

void DrawGUI()
{
	ImGui::Begin(u8"Menu");


	ImGui::End();


//	ImGui::Begin("Debug Window");
//
//	ImGui::Text("Drag Target");
//
//	if (ImGui::BeginDragDropTarget())
//	{
//		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragIndexButton"))
//		{
//			DragList.push_back(*(const int*)payload->Data);
//		}
//		ImGui::EndDragDropTarget();
//	}
//
//
//	for (size_t i = 0; i < DragList.size(); i++)
//	{
//		if (ImGui::Button(std::to_string(DragList.at(i)).c_str()))
//		{
//			DragList.erase(DragList.begin() + i);
//		}
//		if (i + 1 < DragList.size())
//		{
//			ImGui::SameLine();
//		}
//	}
//
//	ImGui::End();


//	ImGui::Begin("TextEditor");
//	te->Render("TextED");
//	ImGui::End();

}