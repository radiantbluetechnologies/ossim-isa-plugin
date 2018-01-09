# THIS REPO HAS BEEN SUPERCEDED BY ossim-msp-plugin, ossim-isa, and ossim-plugins/atp

# ossim-isa-plugin
Contains OSSIM plugin in support of 3DISA MERIT project. 

Instructions for Building OSSIM with MSP

1. Untar the msp-1.5.tgz into some installation directory, say ${OSSIM_DEPENDENCIES}.

2. There will be two subdirectories in msp-1.5: ToolKit and SMDK. Only the former is used.

3. In the OSSIM build configuration script you use, export the following environment variables:

  `export MSP_HOME=${OSSIM_DEPENDENCIES}/msp-1.5/ToolKit`
  `export BUILD_MSP_PLUGIN=ON`
  `export BUILD_3DISA=ON`
  `export CSM_PLUGIN_DIR=${MSP_HOME}/plugins/`
  `export MSP_DATA_DIR=${OSSIM_DEPENDENCIES}/msp-1.5/ToolKit/data/`
  
  Note the trailing slashes on the last two vars above. It won't work without them. 
  
5. There is a test CLI app, `msp-test`, in ossim-msp/test that will print out the list of supported
  sensor models. First make sure that the shell you are running in has the `LOAD_LIBRARY_PATH`
  (or equivalent mechanism) includes the paths to the ossim build/lib directory, as well as 
  `$MSP_HOME/lib`. It should also have the env vars defined in step 3.
  
