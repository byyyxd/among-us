#include "includes.h"
#include <codecvt>

std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device *pDevice = NULL;
ID3D11DeviceContext *pContext = NULL;
ID3D11RenderTargetView *mainRenderTargetView;

HANDLE process_handle;


//CHEAT OPTIONS
bool show = true;





void InitImGui( )
{
	ImGui::CreateContext( );
	ImGuiIO &io = ImGui::GetIO( );
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init( window );
	ImGui_ImplDX11_Init( pDevice, pContext );


}

LRESULT __stdcall WndProc( const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{

	if ( true && ImGui_ImplWin32_WndProcHandler( hWnd, uMsg, wParam, lParam ) )
		return true;

	return CallWindowProc( oWndProc, hWnd, uMsg, wParam, lParam );

}


bool speedchanger;
float speed = 1.f;

bool votingtime;
float amount = 1;

bool discussingtime;
float amountdisc = 1;

bool impostorvision;

bool killcooldown;
int numofimpostors = 0;
int cooldown;
int playercount = 0;

uintptr_t GetPointerAddress( uintptr_t ptr, std::vector<unsigned int> offsets )
{
	uintptr_t addr = ptr;
	for ( uintptr_t i = 0; i < offsets.size( ); ++i )
	{
		addr = *( uintptr_t * ) addr;
		addr += offsets[ i ];
	}
	return addr;
}

template<typename T> void wpm( uintptr_t address, T value )
{
	try
	{
		*( T * ) address = value;
	}
	catch ( ... )
	{
		return;
	}
}

bool init = false;
HRESULT __stdcall hkPresent( IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags )
{
	if ( !init )
	{
		if ( SUCCEEDED( pSwapChain->GetDevice( __uuidof( ID3D11Device ), ( void ** ) &pDevice ) ) )
		{
			pDevice->GetImmediateContext( &pContext );
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc( &sd );
			window = sd.OutputWindow;
			ID3D11Texture2D *pBackBuffer;
			pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID * ) &pBackBuffer );
			pDevice->CreateRenderTargetView( pBackBuffer, NULL, &mainRenderTargetView );
			pBackBuffer->Release( );
			oWndProc = ( WNDPROC ) SetWindowLongPtr( window, GWLP_WNDPROC, ( LONG_PTR ) WndProc );
			InitImGui( );
			init = true;

		}

		else
			return oPresent( pSwapChain, SyncInterval, Flags );
	}


	if ( GetAsyncKeyState( VK_END ) )
	{
		kiero::shutdown( );
		return 0;
	}

	if ( GetAsyncKeyState( VK_INSERT ) & 1 )
	{
		show = !show;
	}

	if ( show )
	{
		ImGui_ImplDX11_NewFrame( );
		ImGui_ImplWin32_NewFrame( );
		ImGui::NewFrame( );
		ImGui::SetNextWindowSize( { 325, 0 } );
		ImGui::Begin( "kawaiiware: Among Us" );
		ImGui::SetWindowSize( { 325, 0 }, ImGuiCond_Always );
		ImGui::SetNextWindowSize( { 325, 0 } );
		ImGui::StyleColorsDark( );


		uintptr_t ModuleBaseAddress = ( uintptr_t ) GetModuleHandleA( "GameAssembly.dll" );
		uintptr_t Address = ModuleBaseAddress + 0x02C6C310;

		//numofimpostors = *( int * ) ( 0x1C );

		//playercount =  *( int * ) ( 0xC ) ;

		ImGui::Text( "Player:" );
		ImGui::Spacing( );
		ImGui::Separator( );

		ImGui::Checkbox( "Speed changer", &speedchanger );
		ImGui::SliderFloat( "Speed Amount", &speed, 1.f, 15.f, "%.1f" );

		ImGui::Checkbox( "Change kill cooldown", &killcooldown );
		ImGui::PushItemWidth( 160.f );
		ImGui::SliderInt( "Cooldown", &cooldown, 1, 30, "%d seconds" );

		ImGui::Spacing( );
		ImGui::Separator( );
		ImGui::Checkbox( "Change voting time", &votingtime ); // this is a client-sided function.
		ImGui::PushItemWidth( 160.f );
		ImGui::SliderFloat( "Vote time", &amount, 1, 60, "%.1f" );

		ImGui::Checkbox( "Impostor vision", &impostorvision );


		std::vector<unsigned int> dwSpeed = { 0x5C, 0x4, 0x14 };
		std::vector<unsigned int> dwDiscussionTime = { 0x5C, 0x4, 0x44 };
		std::vector<unsigned int> dwVotingTime = { 0x5C, 0x4, 0x48 };
		std::vector<unsigned int> dwKillCooldown = { 0x5C, 0x4, 0x20 };
		std::vector<unsigned int> dwImposterVision = { 0x5C, 0x4, 0x1C };

		if ( speedchanger )
		{
			if ( speed != 1.f )
			{
				wpm<float>( GetPointerAddress( Address, dwSpeed ), speed );
			}
		}

		if ( impostorvision )
		{
			wpm<bool>( GetPointerAddress( Address, dwImposterVision ), true );
		}

		if ( killcooldown )
		{
			wpm<int>( GetPointerAddress( Address, dwKillCooldown ), cooldown );
		}

		if ( votingtime )
		{
			if ( amount >= 1 )
			{
				wpm<float>( GetPointerAddress( Address, dwVotingTime ), amount ); // this is a client-sided function.
			}
		}

		if ( discussingtime )
		{
			if ( amountdisc >= 1 )
			{
				wpm<float>( GetPointerAddress( Address, dwDiscussionTime ), amount ); // this is a client-sided function.
			}
		}

		ImGui::End( );

		ImGui::Render( );

		pContext->OMSetRenderTargets( 1, &mainRenderTargetView, NULL );
		ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );
	}
	return oPresent( pSwapChain, SyncInterval, Flags );
}

DWORD WINAPI MainThread( LPVOID lpReserved )
{
	bool init_hook = false;
	do
	{
		if ( kiero::init( kiero::RenderType::D3D11 ) == kiero::Status::Success )
		{
			kiero::bind( 8, ( void ** ) &oPresent, hkPresent );
			init_hook = true;
		}
	} while ( !init_hook );
	return TRUE;
}

BOOL WINAPI DllMain( HMODULE hMod, DWORD dwReason, LPVOID lpReserved )
{
	switch ( dwReason )
	{
		case DLL_PROCESS_ATTACH:
			DisableThreadLibraryCalls( hMod );
			CreateThread( nullptr, 0, MainThread, hMod, 0, nullptr );
			break;
		case DLL_PROCESS_DETACH:
			kiero::shutdown( );
			break;
	}
	return TRUE;
}
