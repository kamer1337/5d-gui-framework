/**
 * Rich Text and Data Grid Demo - Demonstrates the newly added widgets
 * Shows: RichTextBox, RichTextLabel, DataGrid with sorting, filtering, and editing
 */

#include "../include/SDK/SDK.h"
#include "../include/SDK/RichText.h"
#include "../include/SDK/DataGrid.h"
#include <sstream>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Global window pointer for message handling
SDK::Window* g_window = nullptr;
std::shared_ptr<SDK::DataGrid> g_dataGrid = nullptr;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Initialize SDK
    SDK::Initialize();
    
    // Register window class
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"RichTextDataGridDemoClass";
    RegisterClassExW(&wc);
    
    // Create window
    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED,
        L"RichTextDataGridDemoClass",
        L"Rich Text and Data Grid Demo",
        WS_OVERLAPPEDWINDOW,
        100, 100, 1200, 800,
        nullptr, nullptr, hInstance, nullptr
    );
    
    if (!hwnd) {
        MessageBoxW(nullptr, L"Failed to create window!", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    // Register with SDK
    g_window = SDK::WindowManager::GetInstance().RegisterWindow(hwnd);
    
    // Apply theme
    auto theme = std::make_shared<SDK::Theme>(SDK::Theme::CreateModernTheme());
    g_window->SetTheme(theme);
    g_window->SetDepth(SDK::WindowDepth::FOREGROUND);
    g_window->UpdateAppearance();
    
    // ========== Rich Text Demonstrations ==========
    
    // Label
    auto titleLabel = std::make_shared<SDK::Label>(L"Rich Text Features:");
    titleLabel->SetPosition(20, 20);
    titleLabel->SetSize(200, 25);
    titleLabel->SetFontSize(16);
    titleLabel->SetFontBold(true);
    g_window->AddWidget(titleLabel);
    
    // RichTextLabel with markdown
    auto richLabel = std::make_shared<SDK::RichTextLabel>();
    richLabel->SetBounds(20, 50, 550, 150);
    
    // Parse markdown content
    richLabel->GetDocument()->ParseMarkdown(
        L"# Welcome to Rich Text!\n\n"
        L"This is **bold text** and this is *italic text*.\n\n"
        L"You can create [hyperlinks](https://example.com) too!\n\n"
        L"### Features:\n"
        L"- Text formatting (bold, italic, underline)\n"
        L"- Hyperlink support\n"
        L"- Markdown parsing"
    );
    
    g_window->AddWidget(richLabel);
    
    // RichTextBox - Editable
    auto richTextBox = std::make_shared<SDK::RichTextBox>();
    richTextBox->SetBounds(20, 220, 550, 200);
    richTextBox->SetEditable(true);
    richTextBox->SetReadOnly(false);
    
    // Add some formatted content
    auto doc = richTextBox->GetDocument();
    doc->Clear();
    doc->AddText(L"Editable Rich Text Box\n\n", true, false);
    doc->AddText(L"This is ", false, false);
    doc->AddText(L"bold", true, false);
    doc->AddText(L" and this is ", false, false);
    doc->AddText(L"italic", false, true);
    doc->AddText(L".\n\n", false, false);
    doc->AddLink(L"Click here to visit example.com", L"https://example.com");
    doc->AddText(L"\n\nYou can edit this text!", false, false);
    
    // Set link click callback
    richTextBox->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
        if (e == SDK::WidgetEvent::CLICK && data) {
            const wchar_t* url = static_cast<const wchar_t*>(data);
            std::wstring msg = L"Link clicked: ";
            msg += url;
            MessageBoxW(nullptr, msg.c_str(), L"Link Clicked", MB_OK | MB_ICONINFORMATION);
        }
    });
    
    g_window->AddWidget(richTextBox);
    
    // ========== Data Grid Demonstration ==========
    
    // Label
    auto gridTitleLabel = std::make_shared<SDK::Label>(L"Data Grid Features:");
    gridTitleLabel->SetPosition(600, 20);
    gridTitleLabel->SetSize(200, 25);
    gridTitleLabel->SetFontSize(16);
    gridTitleLabel->SetFontBold(true);
    g_window->AddWidget(gridTitleLabel);
    
    // Create DataGrid
    g_dataGrid = std::make_shared<SDK::DataGrid>();
    g_dataGrid->SetBounds(600, 50, 560, 400);
    
    // Add columns
    g_dataGrid->AddColumn(L"ID", 50);
    g_dataGrid->AddColumn(L"Name", 150);
    g_dataGrid->AddColumn(L"Age", 60);
    g_dataGrid->AddColumn(L"Department", 150);
    g_dataGrid->AddColumn(L"Salary", 100);
    
    // Make some columns editable
    g_dataGrid->SetColumnEditable(1, true); // Name
    g_dataGrid->SetColumnEditable(2, true); // Age
    g_dataGrid->SetColumnEditable(4, true); // Salary
    
    // Add sample data
    g_dataGrid->AddRow({L"1", L"John Smith", L"32", L"Engineering", L"$85,000"});
    g_dataGrid->AddRow({L"2", L"Jane Doe", L"28", L"Marketing", L"$72,000"});
    g_dataGrid->AddRow({L"3", L"Bob Johnson", L"45", L"Sales", L"$95,000"});
    g_dataGrid->AddRow({L"4", L"Alice Williams", L"35", L"Engineering", L"$92,000"});
    g_dataGrid->AddRow({L"5", L"Charlie Brown", L"29", L"HR", L"$68,000"});
    g_dataGrid->AddRow({L"6", L"Diana Prince", L"31", L"Engineering", L"$88,000"});
    g_dataGrid->AddRow({L"7", L"Edward Norton", L"42", L"Finance", L"$105,000"});
    g_dataGrid->AddRow({L"8", L"Fiona Apple", L"27", L"Marketing", L"$70,000"});
    g_dataGrid->AddRow({L"9", L"George Lucas", L"50", L"Management", L"$120,000"});
    g_dataGrid->AddRow({L"10", L"Helen Mirren", L"38", L"Sales", L"$90,000"});
    
    // Set selection mode
    g_dataGrid->SetSelectionMode(SDK::DataGrid::SelectionMode::SINGLE);
    
    // Set virtual scrolling for large datasets
    g_dataGrid->SetVirtualScrolling(true);
    
    // Customize appearance
    g_dataGrid->SetHeaderColor(SDK::Color(220, 230, 240, 255));
    g_dataGrid->SetAlternateRowColor(SDK::Color(245, 248, 250, 255));
    g_dataGrid->SetSelectionColor(SDK::Color(180, 210, 255, 255));
    
    // Set callbacks
    g_dataGrid->SetCellClickCallback([](int row, int column) {
        std::wstringstream ss;
        ss << L"Cell clicked: Row " << row << L", Column " << column;
        // This would show a message, but we'll just ignore it in the demo
    });
    
    g_dataGrid->SetCellEditCallback([](int row, int column, const std::wstring& oldValue, const std::wstring& newValue) {
        std::wstringstream ss;
        ss << L"Cell edited at Row " << row << L", Column " << column << L"\n"
           << L"Old value: " << oldValue << L"\n"
           << L"New value: " << newValue;
        MessageBoxW(nullptr, ss.str().c_str(), L"Cell Edited", MB_OK | MB_ICONINFORMATION);
    });
    
    g_dataGrid->SetSortCallback([](int column, SDK::DataGrid::SortOrder order) {
        std::wstring orderStr;
        switch (order) {
            case SDK::DataGrid::SortOrder::ASCENDING: orderStr = L"Ascending"; break;
            case SDK::DataGrid::SortOrder::DESCENDING: orderStr = L"Descending"; break;
            default: orderStr = L"None"; break;
        }
        
        std::wstringstream ss;
        ss << L"Sorted by column " << column << L" in " << orderStr << L" order";
        // This would show a message, but we'll just ignore it in the demo
    });
    
    g_window->AddWidget(g_dataGrid);
    
    // Instructions label
    auto instructionsLabel = std::make_shared<SDK::Label>(L"Instructions:");
    instructionsLabel->SetPosition(600, 465);
    instructionsLabel->SetSize(500, 25);
    instructionsLabel->SetFontBold(true);
    g_window->AddWidget(instructionsLabel);
    
    auto instr1 = std::make_shared<SDK::Label>(L"• Click column headers to sort");
    instr1->SetPosition(600, 490);
    instr1->SetSize(500, 20);
    g_window->AddWidget(instr1);
    
    auto instr2 = std::make_shared<SDK::Label>(L"• Select rows by clicking");
    instr2->SetPosition(600, 510);
    instr2->SetSize(500, 20);
    g_window->AddWidget(instr2);
    
    auto instr3 = std::make_shared<SDK::Label>(L"• Press F2 to edit selected cell (Name, Age, Salary editable)");
    instr3->SetPosition(600, 530);
    instr3->SetSize(500, 20);
    g_window->AddWidget(instr3);
    
    auto instr4 = std::make_shared<SDK::Label>(L"• Use arrow keys to navigate");
    instr4->SetPosition(600, 550);
    instr4->SetSize(500, 20);
    g_window->AddWidget(instr4);
    
    auto instr5 = std::make_shared<SDK::Label>(L"• Click hyperlinks in rich text");
    instr5->SetPosition(600, 570);
    instr5->SetSize(500, 20);
    g_window->AddWidget(instr5);
    
    // Filter controls
    auto filterLabel = std::make_shared<SDK::Label>(L"Filter (type in text box):");
    filterLabel->SetPosition(20, 440);
    filterLabel->SetSize(200, 20);
    g_window->AddWidget(filterLabel);
    
    auto filterBox = std::make_shared<SDK::TextBox>();
    filterBox->SetBounds(20, 465, 300, 30);
    filterBox->SetPlaceholder(L"Type to filter grid...");
    filterBox->SetId(2001);
    
    // Set filter callback
    filterBox->SetEventCallback([](SDK::Widget* w, SDK::WidgetEvent e, void* data) {
        if (e == SDK::WidgetEvent::TEXT_CHANGED && g_dataGrid) {
            SDK::TextBox* box = dynamic_cast<SDK::TextBox*>(w);
            if (box) {
                g_dataGrid->SetFilter(box->GetText());
            }
        }
    });
    
    g_window->AddWidget(filterBox);
    
    // Show window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            if (g_window) {
                // Render all widgets
                for (auto& widget : g_window->GetWidgets()) {
                    widget->Render(hdc);
                }
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            
            if (g_window) {
                for (auto& widget : g_window->GetWidgets()) {
                    if (widget->HandleMouseDown(x, y, 0)) {
                        InvalidateRect(hwnd, nullptr, FALSE);
                        break;
                    }
                }
            }
            return 0;
        }
        
        case WM_LBUTTONUP: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            
            if (g_window) {
                for (auto& widget : g_window->GetWidgets()) {
                    if (widget->HandleMouseUp(x, y, 0)) {
                        InvalidateRect(hwnd, nullptr, FALSE);
                        break;
                    }
                }
            }
            return 0;
        }
        
        case WM_MOUSEMOVE: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            
            if (g_window) {
                for (auto& widget : g_window->GetWidgets()) {
                    if (widget->HandleMouseMove(x, y)) {
                        InvalidateRect(hwnd, nullptr, FALSE);
                        break;
                    }
                }
            }
            return 0;
        }
        
        case WM_KEYDOWN: {
            if (g_dataGrid) {
                if (g_dataGrid->HandleKeyDown((int)wParam)) {
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
            }
            return 0;
        }
        
        case WM_CHAR: {
            if (g_dataGrid) {
                if (g_dataGrid->HandleChar((wchar_t)wParam)) {
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
            }
            return 0;
        }
        
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
