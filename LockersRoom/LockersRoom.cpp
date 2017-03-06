#include "ResourceManager.h"

int main()
{
	CResourceManager ResourceManager;
	std::string UserName;

	while (true)
	{
		int input(0);
		std::cout << "\nEnter '0' - Exit; '1' - User Services; '2' - Admin Services  ";
		std::cin >> input;

		switch (input)
		{
		case 0:
			return 0;

		case 1:
			std::cout << "\nEnter User's name: ";
			std::cin >> UserName;
			for (bool StayInLoop  = true; StayInLoop;)
			{
				std::cout << "\nEnter\n'0' - Exit\n'1' -Quit User Services\n'2' - Give key to user\n'3' - Give master key to user\n'4' - Get key from user\n'5' - Open Door for user\n'6' - Open Slot for user\n";
				std::cin >> input;

				switch (input)
				{
				case 0:
					return 0;
				case 1:
					StayInLoop  = false;
					break;
				case 2:
					ResourceManager.GiveKeyToUser(UserName);
					break;
				case 3:
					ResourceManager.GiveMasterKeyToUser(UserName);
					break;
				case 4:
					ResourceManager.GetKeyFromUser(UserName);
					break;
				case 5:
					ResourceManager.OpenDoorForUser(UserName);
					break;
				case 6:
					std::cout << "\nEnter Slot #: ";
					std::cin >> input;
					ResourceManager.OpenSlotForUser(UserName, input);
					break;
				}
			}
			break;

		case 2:
			for (bool StayInLoop = true; StayInLoop;)
			{
				std::cout << "\nEnter '0' - Exit; '1' - Quit Admin Services; '2' - Users report; '3' - Slots report ";
				std::cin >> input;

				switch (input)
				{
				case 0:
					return 0;
				case 1:
					StayInLoop  = false;
					break;
				case 2:
					ResourceManager.ListUsers();
					break;
				case 3:
					ResourceManager.ListSlots();
					break;
				}
			}
			break;
		}
	}

	return 0;
}
