#include "TopMenuBar.h"

#include <imgui.h>

#include "AppEvents.h"

TopMenuBar::TopMenuBar()
	: BaseWindow("TopMenu", true, false),
    showAxes(true),
    showGrid(true),
    currentShadingMode(0)
{}

void TopMenuBar::render()
{
	if (!isVisible) return;

    if (ImGui::BeginMainMenuBar())
    {
        // ==========================================
        // MENU: FILE
        // ==========================================
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Nuovo Progetto"))
                EventDispatcher<>::getInstance().dispatch(AppEvents::MENU_NEW_PROJECT);

            if (ImGui::MenuItem("Apri Heightmap..."))
                EventDispatcher<>::getInstance().dispatch(AppEvents::MENU_OPEN_HEIGHTMAP);

            ImGui::Separator();

            if (ImGui::MenuItem("Salva Progetto", "Ctrl+S"))
                EventDispatcher<>::getInstance().dispatch(AppEvents::MENU_SAVE_PROJECT);

            if (ImGui::MenuItem("Salva con nome..."))
                EventDispatcher<>::getInstance().dispatch(AppEvents::MENU_SAVE_AS);

            ImGui::Separator();

            if (ImGui::MenuItem("Esporta Mesh 3D (.obj)"))
                EventDispatcher<>::getInstance().dispatch(AppEvents::MENU_EXPORT_MESH);

            if (ImGui::MenuItem("Esporta Heightmap (.exr)", "Ctrl+E"))
                EventDispatcher<>::getInstance().dispatch(AppEvents::MENU_EXPORT_HEIGHTMAP);

            ImGui::Separator();

            if (ImGui::MenuItem("Esci", "Alt+F4"))
                EventDispatcher<>::getInstance().dispatch(AppEvents::APP_EXIT);

            ImGui::EndMenu();
        }

        // ==========================================
        // MENU: EDIT
        // ==========================================
        if (ImGui::BeginMenu("Modifica"))
        {
            if (ImGui::MenuItem("Annulla (Undo)", "Ctrl+Z"))
                EventDispatcher<>::getInstance().dispatch(AppEvents::MENU_UNDO);

            if (ImGui::MenuItem("Ripristina (Redo)", "Ctrl+Y"))
                EventDispatcher<>::getInstance().dispatch(AppEvents::MENU_REDO);

            ImGui::Separator();

            if (ImGui::MenuItem("Opzioni Editor..."))
                EventDispatcher<>::getInstance().dispatch(AppEvents::MENU_OPTIONS);

            ImGui::EndMenu();
        }

        // ==========================================
        // MENU: VIEW
        // ==========================================
        if (ImGui::BeginMenu("Visualizza"))
        {
            if (ImGui::BeginMenu("Modalità Shading"))
            {
                // Mostra un pallino accanto alla modalità attiva (usando currentShadingMode)
                if (ImGui::MenuItem("Realistico", nullptr, currentShadingMode == 0))
                    EventDispatcher<>::getInstance().dispatch(AppEvents::MENU_SHADING_REAL);

                if (ImGui::MenuItem("Wireframe", nullptr, currentShadingMode == 1))
                    EventDispatcher<>::getInstance().dispatch(AppEvents::MENU_SHADING_WIREFRAME);

                if (ImGui::MenuItem("Normali", nullptr, currentShadingMode == 2))
                    EventDispatcher<>::getInstance().dispatch(AppEvents::MENU_SHADING_NORMALS);

                if (ImGui::MenuItem("Colori Altitudine", nullptr, currentShadingMode == 3))
                    EventDispatcher<>::getInstance().dispatch(AppEvents::MENU_SHADING_ALTITUDE);

                ImGui::EndMenu();
            }

            ImGui::Separator();

            // Checkbox integrata nel menu
            if (ImGui::MenuItem("Mostra Griglia", nullptr, &showGrid))
                EventDispatcher<>::getInstance().dispatch(AppEvents::MENU_TOGGLE_GRID);

            if (ImGui::MenuItem("Mostra Assi XYZ", nullptr, &showAxes))
                EventDispatcher<>::getInstance().dispatch(AppEvents::MENU_TOGGLE_AXES);

            ImGui::Separator();

            if (ImGui::MenuItem("Reset Camera"))
                EventDispatcher<>::getInstance().dispatch(AppEvents::MENU_RESET_CAMERA);

            if (ImGui::MenuItem("Ripristina Layout UI"))
                EventDispatcher<>::getInstance().dispatch(AppEvents::MENU_RESET_LAYOUT);

            ImGui::EndMenu();
        }

        // ==========================================
        // MENU: TERRAIN
        // ==========================================
        if (ImGui::BeginMenu("Terreno"))
        {
            if (ImGui::MenuItem("Ridimensiona Terreno..."))
                EventDispatcher<>::getInstance().dispatch(AppEvents::MENU_RESIZE_TERRAIN);

            if (ImGui::MenuItem("Genera da Rumore (Noise)..."))
                EventDispatcher<>::getInstance().dispatch(AppEvents::MENU_GENERATE_NOISE);

            ImGui::Separator();

            if (ImGui::MenuItem("Azzera Altitudini"))
                EventDispatcher<>::getInstance().dispatch(AppEvents::MENU_FLATTEN_TERRAIN);

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}
