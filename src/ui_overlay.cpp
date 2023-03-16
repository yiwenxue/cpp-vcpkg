#include "ui_overlay.hpp"

#include <algorithm>
#include <boost/fusion/adapted/std_array.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/as_vector.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/mpl.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/fusion/sequence/intrinsic_fwd.hpp>
#include <boost/fusion/sequence/sequence_facade.hpp>
#include <boost/mpl/range_c.hpp>

#include <functional>
#include <imgui.h>

#include <iostream>
#include <string>
#include <type_traits>

template <typename Struct>
void ImGuiReflectField(const char *name, Struct &value);

template <typename Struct> struct ImGuiMemberVisitor {
  ImGuiMemberVisitor(Struct &s) : s(s) {}
  template <typename Index> void operator()(Index i) {
    using namespace boost::fusion;
    std::string name = extension::struct_member_name<Struct, i>::call();
    ImGuiReflectField(name.c_str(), at<boost::mpl::int_<Index::value>>(s));
  }

private:
  Struct &s;
};

template <typename...> using void_t = void;

template <typename T, class = void> struct is_container : std::false_type {};

template <typename T>
struct is_container<T, void_t<typename T::iterator, typename T::const_iterator>>
    : std::true_type {};

template <typename Struct>
void ImGuiReflectField(const char *name, Struct &value) {
  if constexpr (std::is_same_v<Struct, int>) {
    ImGui::InputInt(name, &value);
  } else if constexpr (std::is_same_v<Struct, float>) {
    ImGui::InputFloat(name, &value);
  } else if constexpr (std::is_same_v<Struct, float2>) {
    ImGui::InputFloat2(name, &value.x);
  } else if constexpr (std::is_same_v<Struct, float3>) {
    ImGui::InputFloat3(name, &value.x);
  } else if constexpr (std::is_same_v<Struct, float4>) {
    ImGui::InputFloat4(name, &value.x);
  } else if constexpr (is_container<Struct>::value) {
    if (ImGui::BeginListBox(name)) {
      uint32_t count = 0;
      for (auto &item : value) {
        ImGuiReflectField(std::to_string(count++).c_str(), item);
      }
      ImGui::EndListBox();
    }
  } else {
    std::cout << "ImGuiReflectField: unhandled type " << typeid(Struct).name()
              << " for " << name << std::endl;
  }
}

template <typename Struct>
void ImGuiReflectStruct(const char *name, Struct &s) {
  if (name == nullptr) {
    name = typeid(Struct).name();
  }
  using namespace boost::fusion;
  using namespace boost::mpl;
  typedef range_c<std::size_t, 0, result_of::size<Struct>::value> Indices;

  // sub-structs, using node
  if (ImGui::TreeNode(name)) {
    for_each(Indices(), ImGuiMemberVisitor<Struct>(s));
    ImGui::TreePop();
  }
}

BOOST_FUSION_ADAPT_STRUCT(float2, x, y)
BOOST_FUSION_ADAPT_STRUCT(float3, x, y, z)
BOOST_FUSION_ADAPT_STRUCT(float4, x, y, z, w)
BOOST_FUSION_ADAPT_STRUCT(vertex, pos, normal, uv0, uv1)

void render_ui_overlay() {
  static int i = 0;
  static float f = 0.0f;
  static float2 f2{1.0f, 2.0f};
  static float3 f3{1.0f, 2.0f, 3.0f};
  static float4 f4{1.0f, 2.0f, 3.0f, 4.0f};

  static std::vector<float3> f3s{{1.0f, 2.0f, 3.0f},
                                 {4.0f, 5.0f, 6.0f},
                                 {7.0f, 8.0f, 9.0f},
                                 {10.0f, 11.0f, 12.0f}};

  ImGui::Begin("UI Overlay");

  ImGuiReflectField("i", i);
  ImGuiReflectField("f", f);
  ImGuiReflectField("f2", f2);
  ImGuiReflectField("f3", f3);
  ImGuiReflectField("f4", f4);
  ImGuiReflectField("f3s", f3s);

  ImGui::End();
}