#pragma once

struct float2 {
  float x, y;
};

struct float3 {
  float x, y, z;
};

struct float4 {
  float x, y, z, w;
};

struct vertex {
  float3 pos;
  float3 normal;
  float2 uv0;
  float2 uv1;
};

void render_ui_overlay();