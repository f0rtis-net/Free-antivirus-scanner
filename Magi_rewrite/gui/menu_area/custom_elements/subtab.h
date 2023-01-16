#pragma once
#include "../../../libs/imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../../../libs/imgui/imgui_internal.h"
#include <map>
#include <algorithm>

namespace gui
{
	__forceinline double InOutBounce( const double t )
	{
		if ( t < 0.5 )
		{
			return 8 * pow( 2, 8 * ( t - 1 ) ) * abs( sin( t * 3.14 * 7 ) );
		}
		else
		{
			return 1 - 8 * pow( 2, -8 * t ) * abs( sin( t * 3.14 * 7 ) );
		}
	}

	bool subtab( const char* text, const ImVec2& elem_size, bool activated, const char* icon, ImFont* ico_font, bool& need_inform )
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow( );
		if ( window->SkipItems )
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID( text );
		const ImVec2 label_size = ImGui::CalcTextSize( text, NULL, true );
		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = ImGui::CalcItemSize( elem_size, label_size.x, label_size.y + style.FramePadding.y * 2.0f );

		const ImRect bb( pos, ImVec2( pos.x + size.x, pos.y + size.y ) );
		ImGui::ItemSize( ImVec2( elem_size.x, size.y + 20 ), style.FramePadding.y );
		if ( !ImGui::ItemAdd( bb, id ) )
			return false;

		static std::map<ImGuiID, float> selected_animation;
		auto it_selected = selected_animation.find( ImGui::GetItemID( ) );

		if ( it_selected == selected_animation.end( ) )
		{
			selected_animation.insert( { ImGui::GetItemID( ), {0.5f} } );
			it_selected = selected_animation.find( ImGui::GetItemID( ) );
		}

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, 0 );
		ImColor pressed_col = ImVec4( 46 / 255.f, 218 / 255.f, 172 / 255.f, activated ? 1.f : ImClamp( it_selected->second = hovered ? it_selected->second + ImGui::GetIO( ).DeltaTime : 0.f, 0.f, 1.f ) );
		ImColor non_active = ImColor( 64, 81, 121 );

		window->DrawList->AddRectFilled( ImVec2( bb.Max.x - 2, bb.Max.y ), ImVec2( bb.Max.x, bb.Min.y ), pressed_col );

		auto y_calc = bb.Min.y + elem_size.y / 2 - ImGui::CalcTextSize( icon ).y / 2 - 10;

		if ( need_inform )
		{
			window->DrawList->AddCircleFilled( ImVec2( bb.Min.x + bb.GetWidth( ) - 30, bb.Min.y + 10 ), 3.f, ImColor( 1.f, 165 / 255.f, 0.f ) );

			if ( pressed )
				need_inform = false;
		}

		ImGui::PushFont( ico_font );
		window->DrawList->AddText( ImVec2( bb.Min.x + elem_size.x / 2 - ImGui::CalcTextSize( icon ).x / 2, y_calc ),
			hovered || activated ? pressed_col : ImColor( 64, 81, 121 ), icon );
		ImGui::PopFont( );

		window->DrawList->AddText( ImVec2( bb.Min.x + elem_size.x / 2 - ImGui::CalcTextSize( text ).x / 2 + 1, bb.Max.y - 15 ),
			hovered || activated ? pressed_col : ImColor( 64, 81, 121 ), text );

		return pressed;
	}

	void circle_progress( ImVec2 center_pos, float radius, ImColor background, ImColor foreground )
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow( );
		if ( window->SkipItems )
			return;

		auto front = ImGui::GetForegroundDrawList( );

		static float arc_size = 0.45f;
		static float thickness = 4.f;

		// Animation
		static float position = 0.f;
		position = ImLerp( position, IM_PI * 2.f, ImGui::GetIO( ).DeltaTime * 3.3f );

		// Background
		front->PathClear( );
		front->PathArcTo( window->DC.CursorPos + center_pos, radius, 0.f, 2.f * IM_PI, 40.f );
		front->PathStroke( ImGui::GetColorU32( background.Value ), 0, thickness );

		// Foreground
		front->PathClear( );
		front->PathArcTo( window->DC.CursorPos + center_pos, radius, IM_PI * 1.5f + position, IM_PI * ( 1.5f + arc_size ) + position, 40.f );
		front->PathStroke( ImGui::GetColorU32( foreground.Value ), 0, thickness );

		// Reset animation
		if ( position >= IM_PI * 1.90f )
			position = 0.f;
	}
}