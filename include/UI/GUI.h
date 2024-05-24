#pragma once
#include "imgUI/imgui.h"
#include "UI/TextEditor.h"
#include <vector>

std::vector <int> DragList;

TextEditor* te;
float currentTime;
// 绘制用户当前位置
void DrawDebugUI(glm::vec3 position){
    ImGui::Begin("Debug Window");
    ImGui::SetWindowFontScale(1.5f); // 增加文本大小的缩放比例
    ImGui::Text("Welcome to the Snow World!");
    ImGui::Separator();
    ImGui::SetWindowFontScale(1.0f); // 增加文本大小的缩放比例
    ImGui::Text("user position : (%f,%f,%f)", position.x, position.y, position.z);
    ImGui::Text("current time : %d : 00", (int)currentTime);
    ImGui::End();
}

void DrawMenuUI(Camera& camera,
                float& scale,
                ImVec4& backgroundColor,
                bool& isSnow,
                glm::vec3& directLightPosition){
    ImGui::Begin(u8"Menu");

#pragma region Camera
    float time = 0;
    ImGui::Text(u8"camera position");
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

    ImGui::Text("directLight position");
    float sumPosition[3] = {directLightPosition.x, directLightPosition.y, directLightPosition.z};
    if (ImGui::SliderFloat("sun X", &sumPosition[0], -100.0f, 100.0f))
    {
        // 当用户修改滑块的值时，更新camera的位置
        directLightPosition.x = sumPosition[0];
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset sunX"))
    {
        directLightPosition.x = 0.0f;
    }

    if (ImGui::SliderFloat("sun Y", &sumPosition[1], -100.0f, 100.0f)){
        directLightPosition.y = sumPosition[1];
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset sunY")){
        directLightPosition.y = 0.0f;
    }

    if (ImGui::SliderFloat("sun Z", &sumPosition[2], -100.0f, 100.0f)){
        directLightPosition.z = sumPosition[2];
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset sunZ")){
        directLightPosition.z = 0.0f;
    }


    // 模拟24小时 太阳的圆周运动
    ImGui::Text("time");
    if (ImGui::SliderFloat("time", &currentTime, 0.0f, 24.0f)){
        directLightPosition.x = 100.0f * cos(currentTime / 24.0f * 2 * 3.1415926);
        directLightPosition.y = 100.0f * sin(currentTime / 24.0f * 2 * 3.1415926);
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