#pragma once

#include <functional>
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

    virtual void render() = 0;

protected:
    std::string name;
};

class Boxes : public Widget {
public:
    Boxes(const std::string &name) : Widget(name) {
    }
    virtual ~Boxes() = default;

    virtual void render() override {
        ImGui::PushID(this->name.c_str());
        if (this->vertical) {
            ImGui::BeginChild("boxes", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysAutoResize);
        } else {
            ImGui::BeginChild("boxes", ImVec2(0, 0), true,
                              ImGuiWindowFlags_AlwaysAutoResize
                                  | ImGuiWindowFlags_HorizontalScrollbar);
        }

        for (auto &widget : this->widgets) {
            widget->render();
        }

        ImGui::EndChild();
        ImGui::PopID();
    }

    void set_vertical(bool vertical) {
        this->vertical = vertical;
    }

    void set_size(uint32_t size) {
        this->widgets.resize(size);
    }

    void set_widget(uint32_t idx, std::unique_ptr<Widget> widget) {
        this->widgets[idx] = std::move(widget);
    }

private:
    bool                                 vertical{true};
    std::vector<std::unique_ptr<Widget>> widgets;
};

class Grid : public Widget {
public:
    Grid(const std::string &name) : Widget(name) {
    }
    virtual ~Grid() = default;

    virtual void render() override {
        ImGui::PushID(this->name.c_str());
        ImGui::BeginChild("grid", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysAutoResize);

        for (auto &box : this->boxes) {
            box->render();
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
            this->boxes[i]->set_size(this->columns);
        }
    }

    void set_columns(uint32_t columns) {
        this->columns = columns;
        for (auto &box : this->boxes) {
            box->set_size(this->columns);
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

class Label : public Widget {
public:
    Label(const std::string &name) : Widget(name) {
    }
    virtual ~Label() = default;

    virtual void render() override {
        ImGui::PushID(this->name.c_str());
        // get the size of the text
        auto size = ImGui::CalcTextSize(this->text.c_str());
        // if the border is set, draw a border around the text
        if (this->border > 0.0f) {
            auto pos = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y),
                                                IM_COL32(255, 255, 255, 255), 0.0f, 0,
                                                this->border);
        }
        if (this->wrap) {
            ImGui::TextWrapped("%s", this->text.c_str());
        } else {
            ImGui::Text("%s", this->text.c_str());
        }
        ImGui::PopID();
    }

    void set_text(std::string text) {
        this->text = std::move(text);
    }

    void set_border(float border) {
        this->border = border;
    }

private:
    float       border{0.0f};
    bool        wrap{false};
    std::string text;
};

class WindowWidget : public Widget {
public:
    WindowWidget(const std::string &name) : Widget(name) {
    }
    virtual ~WindowWidget() = default;

    virtual void render() override {
        ImGui::PushID(this->name.c_str());

        ImGui::Begin(this->title.c_str(), nullptr, ImGuiWindowFlags_NoResize);

        this->root->render();

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

    virtual void render() override {
        ImGui::PushID(this->name.c_str());

        ImGui::Begin(this->title.c_str());
        if (this->root) {
            this->root->render();
        }

        ImGui::End();
        ImGui::PopID();
    }

    virtual void set_title(std::string title) override;

protected:
    Application *application;
};

class ButtonEvent {
public:
    ButtonEvent()          = default;
    virtual ~ButtonEvent() = default;

    virtual void operator()() = 0;
};

class Button : public Widget {
public:
    Button(const std::string &name) : Widget(name) {
    }
    virtual ~Button() = default;

    virtual void render() override {
        ImGui::PushID(this->name.c_str());
        if (ImGui::Button(this->text.c_str())) {
            if (this->callback) {
                this->callback();
            }
        }
        ImGui::PopID();
    }

    void set_text(std::string text) {
        this->text = std::move(text);
    }

    void set_callback(std::function<void()> callback) {
        this->callback = callback;
    }

protected:
    std::string text;

    std::function<void()> callback;
};

class ProgressBar : public Widget {
public:
    ProgressBar(const std::string &name) : Widget(name) {
    }
    virtual ~ProgressBar() = default;

    virtual void render() override {
        ImGui::PushID(this->name.c_str());
        ImGui::ProgressBar(this->progress);
        ImGui::PopID();
    }

    void set_progress(float progress) noexcept {
        this->progress = progress;
    }

    float get_progress() noexcept {
        return this->progress;
    }

protected:
    float progress{0.0f};
};