#pragma once
#include "imgUI/imgui.h"
#include "UI/TextEditor.h"
#include <vector>

std::vector <int> DragList;

TextEditor* te;

// 绘制用户当前位置
void DrawDebugUI(glm::vec3 position){
    ImGui::Begin("Debug Window");
    ImGui::Text("user position : (%f,%f,%f)", position.x, position.y, position.z);
    ImGui::End();
}

void DrawMenuUI(Camera& camera,
                float& scale,
                ImVec4& backgroundColor,
                bool& isSnow,
                glm::vec3 directLightPosition){
    ImGui::Begin(u8"Menu");
//    ImGui::Text("Background Color");
//    ImGui::SameLine();
//    if (ImGui::Button("Reset"))
//    {
//        // 重置背景颜色的值
//        backgroundColor = ImVec4(0.0f,0.0f,0.0f,1.0f);
//    }
    // 使用调色板来更改背景颜色
    if (ImGui::ColorPicker4("##UNIQALID", (float*)&backgroundColor,
                            ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_PickerHueBar,
                            NULL))
    {
        // 如果用户在调色板中选择了颜色，可以在这里处理颜色改变的逻辑
    }
//    ImGui::Text(u8"scale");
//    if (ImGui::SliderFloat("##uniqueID", &scale, -1.0f, 1.0f))
//    {
//        // 当用户滑动滑动条时，这里的代码将被执行，调整大小
//        camera.ProcessMouseScroll(scale);
//    }
//    ImGui::SameLine();
//    // 当按钮被按下时，重置scale的值
//    if (ImGui::Button("Reset"))
//    {
//        // 重置camera的值
//        camera.ProcessMouseScroll(50);
//        scale = 0.0f;
//    }

#pragma region Camera
    ImGui::Text(u8"position");
    // 添加三个滑块来控制camera的位置
    float cameraPosition[3] = {camera.Position.x, camera.Position.y, camera.Position.z};
    if (ImGui::SliderFloat("X", &cameraPosition[0], -100.0f, 100.0f))
    {
        // 当用户修改滑块的值时，更新camera的位置
        camera.Position.x = cameraPosition[0];
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset X"))
    {
        camera.Position.x = 0.0f;
    }

    if (ImGui::SliderFloat(u8"Y", &cameraPosition[1], 5.0f, 100.0f))
    {
        camera.Position.y = cameraPosition[1];
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset Y"))
    {
        camera.Position.y = 0.0f;
    }

    if (ImGui::SliderFloat(u8"Z", &cameraPosition[2], -100.0f, 100.0f))
    {
        camera.Position.z = cameraPosition[2];
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset Z"))
    {
        camera.Position.z = 3.0f;
    }

    ImGui::Text(u8"Switch Snow");
    ImGui::SameLine();
    ImGui::Checkbox("##SwitchSnow", &isSnow);
#pragma endregion
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