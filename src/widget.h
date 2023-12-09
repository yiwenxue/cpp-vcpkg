#pragma once

#include <imgui.h>
#include <memory>
#include <vector>

#include "application.h"

// events that
// declare the widget class base as a gui element that can be rendered
class Widget {
public:
    Widget(const std::string &name) : name(name) {
    }
    virtual ~Widget() = default;

    virtual void render(ImVec2 pos = {0.f, 0.f}, ImVec2 size = {0.f, 0.f}) = 0;

protected:
    std::string name;
};

class Layout {
public:
    enum class Alignment {
        Start, // top or left
        End,   // bottom or right
        Center
    };
    enum class SizePolicy {
        Fixed,     // use the size hint
        Expanding, // use the size hint, but allow expansion
        Auto,      // minimum size
    };

public:
    Layout()          = default;
    virtual ~Layout() = default;

    virtual void set_horizontal_alignment(Alignment alignment) noexcept {
        this->horizontal_alignment = alignment;
    }

    virtual void set_vertical_alignment(Alignment alignment) noexcept {
        this->vertical_alignment = alignment;
    }

    virtual void set_horizontal_size_policy(SizePolicy policy) noexcept {
        this->horizontal_size_policy = policy;
    }

    virtual void set_vertical_size_policy(SizePolicy policy) noexcept {
        this->vertical_size_policy = policy;
    }

    virtual void set_size_hint(ImVec2 size_hint) noexcept {
        this->size_hint = size_hint;
    }

    virtual void set_minimum_size(ImVec2 minimum_size) noexcept {
        this->minimum_size = {std::min(minimum_size.x, this->maximum_size.x),
                              std::min(minimum_size.y, this->maximum_size.y)};
    }

    virtual void set_maximum_size(ImVec2 maximum_size) noexcept {
        this->maximum_size = {std::max(maximum_size.x, this->minimum_size.x),
                              std::max(maximum_size.y, this->minimum_size.y)};
    }

    virtual void set_size(ImVec2 pos = {0.f, 0.f}, ImVec2 size = {0.f, 0.f}) noexcept {
        size = this->minimum_size;
        // set size
        ImVec2 available_size = ImGui::GetContentRegionAvail();
        switch (this->horizontal_size_policy) {
            case SizePolicy::Fixed:
                size.x = this->size_hint.x;
                break;
            case SizePolicy::Expanding:
                size.x = available_size.x;
                break;
            case SizePolicy::Auto:
                size.x = 0;
                break;
        }

        switch (this->vertical_size_policy) {
            case SizePolicy::Fixed:
                size.y = this->size_hint.y;
                break;
            case SizePolicy::Expanding:
                size.y = available_size.y;
                break;
            case SizePolicy::Auto:
                size.y = 0;
                break;
        }

        this->size = size;

        ImGui::SetNextItemWidth(size.x);
    }

    virtual void show_layout_debug() const noexcept {
        // show widget debug box, get draw list and draw a box around the widget
        ImGui::GetWindowDrawList()->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y),
                                            IM_COL32(255, 0, 0, 255), 0.0f, 0, 1.0f);
    }

    virtual void set_color(ImColor color) {
        this->color = color;
    }

    virtual void show_border() {
        if (this->border > 0.0f) {
            ImGui::GetWindowDrawList()->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), color,
                                                0.0f, 0, 1.0f);
        }
    }

    virtual void set_border(float border) {
        this->border = border;
    }

protected:
    Alignment  horizontal_alignment{Alignment::Start};
    Alignment  vertical_alignment{Alignment::Start};
    SizePolicy horizontal_size_policy{SizePolicy::Expanding};
    SizePolicy vertical_size_policy{SizePolicy::Auto};

    ImVec2 size = ImVec2(0, 0);
    ImVec2 pos  = ImVec2(0, 0);

    ImVec2 size_hint    = ImVec2(0, 0);
    ImVec2 minimum_size = ImVec2(0, 0);
    ImVec2 maximum_size = ImVec2(0, 0);

    ImColor color{255, 255, 255, 255};
    float   border{0.0f};
};

class Boxes
    : public Widget
    , public Layout {
public:
    Boxes(const std::string &name) : Widget(name) {
    }
    virtual ~Boxes() = default;

    virtual void render(ImVec2 pos = {0.f, 0.f}, ImVec2 size = {0.f, 0.f}) override {
        set_size(pos, size);
        ImGui::PushID(this->name.c_str());
        if (this->vertical) {
            ImGui::BeginChild("boxes", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysAutoResize);
        } else {
            ImGui::BeginChild("boxes", ImVec2(0, 0), true,
                              ImGuiWindowFlags_AlwaysAutoResize
                                  | ImGuiWindowFlags_HorizontalScrollbar);
        }

        size = ImGui::GetContentRegionAvail();
        for (auto &widget : this->widgets) {
            pos = ImGui::GetCursorPos();
            size.y -= pos.y;
            widget->render(pos, size);
        }

        ImGui::EndChild();
        ImGui::PopID();
    }

    void set_vertical(bool vertical) {
        this->vertical = vertical;
    }

    void set_capacity(uint32_t size) {
        this->widgets.resize(size);
    }

    void set_widget(uint32_t idx, std::unique_ptr<Widget> widget) {
        this->widgets[idx] = std::move(widget);
    }

private:
    bool                                 vertical{true};
    std::vector<std::unique_ptr<Widget>> widgets;
};

class Grid
    : public Widget
    , public Layout {
public:
    Grid(const std::string &name) : Widget(name) {
    }
    virtual ~Grid() = default;

    virtual void render(ImVec2 pos = {0.f, 0.f}, ImVec2 size = {0.f, 0.f}) override {
        set_size(pos, size);
        ImGui::PushID(this->name.c_str());
        ImGui::BeginChild("grid", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysAutoResize);

        for (auto &box : this->boxes) {
            box->render(pos, size);
        }

        ImGui::EndChild();
        ImGui::PopID();
    }

    void set_rows(uint32_t rows) {
        if (this->rows >= rows) {
            return;
        }
        this->boxes.resize(rows);
        for (uint32_t i = this->rows; i < rows; ++i) {
            this->boxes[i] = std::make_unique<Boxes>("box" + std::to_string(i));
            this->boxes[i]->set_capacity(this->columns);
        }
    }

    void set_columns(uint32_t columns) {
        this->columns = columns;
        for (auto &box : this->boxes) {
            box->set_capacity(this->columns);
        }
    }

    void set_widget(uint32_t row, uint32_t column, std::unique_ptr<Widget> widget) {
        this->boxes[row]->set_widget(column, std::move(widget));
    }

protected:
    uint32_t rows    = 0;
    uint32_t columns = 0;

    std::vector<std::unique_ptr<Boxes>> boxes;
};

class Label
    : public Widget
    , public Layout {
public:
    Label(const std::string &name) : Widget(name) {
    }
    virtual ~Label() = default;

    virtual void render(ImVec2 pos = {0.f, 0.f}, ImVec2 size = {0.f, 0.f}) override {
        // get cursor pos
        pos = ImGui::GetCursorPos();
        set_size(pos, size);
        ImGui::PushID(this->name.c_str());
        if (this->wrap) {
            ImGui::TextWrapped("%s", this->text.c_str());
        } else {
            ImGui::Text("%s", this->text.c_str());
        }
        show_border();
        ImGui::PopID();
    }

    void set_text(std::string text) {
        this->text = std::move(text);
    }

private:
    bool        wrap{false};
    std::string text;
};

class WindowWidget
    : public Widget
    , public Layout {
public:
    WindowWidget(const std::string &name) : Widget(name) {
    }
    virtual ~WindowWidget() = default;

    virtual void render(ImVec2 pos = {0.f, 0.f}, ImVec2 size = {0.f, 0.f}) override {
        ImGui::PushID(this->name.c_str());
        set_size(pos, size);
        ImGui::Begin(this->title.c_str());

        this->root->render(pos, size);

        ImGui::End();
        ImGui::PopID();
    }

    virtual void set_title(std::string title) {
        this->title = std::move(title);
    }

    const std::string get_title() const noexcept {
        return this->title;
    }

    void set_widget(std::unique_ptr<Widget> widget) {
        this->root = std::move(widget);
    }

protected:
    std::string title;

    std::unique_ptr<Widget> root;
};

class ApplicationWindow : public WindowWidget {
public:
    ApplicationWindow(const std::string &name, Application *app) :
        WindowWidget(name), application(app) {
    }
    virtual ~ApplicationWindow() = default;

    virtual void render(ImVec2 pos = {0.f, 0.f}, ImVec2 size = {0.f, 0.f}) override {
        set_size(pos, size);
        ImGui::PushID(this->name.c_str());
        ImGui::Begin(this->title.c_str(), nullptr, ImGuiWindowFlags_NoResize);
        if (this->root) {
            this->root->render(pos, size);
        }

        ImGui::End();
        ImGui::PopID();
    }

    virtual void set_title(std::string title) override;

    virtual void set_size(ImVec2 pos = {0.f, 0.f}, ImVec2 size = {0.f, 0.f}) noexcept override;

protected:
    Application *application;
};

class ButtonEvent {
public:
    ButtonEvent()          = default;
    virtual ~ButtonEvent() = default;

    virtual void operator()() = 0;
};

class Button
    : public Widget
    , Layout {
public:
    Button(const std::string &name) : Widget(name) {
    }
    virtual ~Button() = default;

    virtual void render(ImVec2 pos = {0.f, 0.f}, ImVec2 size = {0.f, 0.f}) override {
        ImGui::PushID(this->name.c_str());
        // align to right
        if (ImGui::Button(this->text.c_str())) {
        }
        ImGui::PopID();
    }

    void set_text(std::string text) noexcept {
        this->text = std::move(text);
    }

protected:
    std::string text;
};