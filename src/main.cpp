/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Namiono/Namiono.h>

int main(int argc, char* argv[])
{
	printf("Namiono-Server 0.5b\n");
	Namiono::_Namiono* namiono = new Namiono::_Namiono(argc,argv);

	namiono->Init();
	namiono->Start();
	namiono->Process();
	namiono->Close();

	delete namiono;
	namiono = nullptr;
	
	return 0;
}

