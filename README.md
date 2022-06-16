# FTFS 23

## About
This is a FIFA companion app which allows for you to play Co-op career mode with friends. This has been a feature that has been missing from FIFA games over the years
despite fans of the game requesting for such a game mode, therefore I've decided to try to the best of my ability to conjure up a companion app that is run alongside 
your FIFA game to simulate an experience where you can play a career mode like save with friends despite managing different teams.

## How to build FTFS
I've strived to make the building process of this project very simple and quick to complete, once you have aquired a copy of the repository: <br/>
1. Run the `build.bat` script which can be found in the root repository directory and enter the type of project files to generate (valid inputs can be found on the [Premake website](https://premake.github.io/docs/Using-Premake)).
2. Build the GLFW and Freetype libraries (in the `libs` directory) using the project files generated, which should be found in their respective `build` directories (Make sure when your building the FreeType library, that the runtime library being linked to is /MTd for debug and /MT for release).
4. Once the required libraries have been built, use the FTFS project file generated (found in the root repository directory) to build the FTFS program.

## License
FTFS 23 is released under the terms of the MIT license. See [LICENSE](LICENSE) for more information or see https://opensource.org/licenses/MIT.
