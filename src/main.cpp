#include "WFC.hpp"

int main() {
	WFC::WFC wfc("wfc_data.yaml", true);
	wfc.Start();
	wfc.visual->Render();
}