..
   Copyright (C) 2019-2022, Xilinx, Inc.
   Copyright (C) 2022-2026, Advanced Micro Devices, Inc.
   
   `Terms and Conditions <https://www.amd.com/en/corporate/copyright>`_.

.. _CONFIGURATION:

Configuring the Library Elements
---------------------------------

**Prerequisites** — add the Python binary for `config_helper.py` to your PATH:

.. code-block::

	setenv PATH "<your-Vitis-install-path>/lin64/2025.1/Vitis/aietools/tps/lnx64/python-3.13.0/bin:$PATH"

Configure DSPLIB IPs using the Python script `config_helper.py`. This script guides you to a valid configuration for any DSPLIB IP through the console interface.

For each parameter, the config helper prints a legal set or range of values and prompts you to enter one.

- If the value is legal, the config helper moves to the next parameter.
- If the value is not legal, the config helper returns an error and prompts you to choose from the legal set or range.

To return to the previous parameter at any time, enter ``z`` or ``Z``.

After all parameters are set, the config helper outputs a top-level file:

- **AI Engine IPs**: a `graph_*ip_name_instance_name*.txt` file containing a top-level graph class to instantiate the IP. Use the PRINT_GRAPH option to print the graph to the console.
- **VSS IPs**: a `cfg_*ip_name_instance_name*.cfg` file. The "PART" parameter represents the device the VSS generator supports, not an exact part name. Accept the default part name for your target device, extract the remaining parameters using config_helper, then edit the part name in the output .cfg file to the specific part before building the VSS.

Pass the .cfg file as input to the top-level VSS Makefile. To skip generating a top-level instance file, call `config_helper.py` with the NO_INSTANCE argument.

Running Config Helper
^^^^^^^^^^^^^^^^^^^^^

`config_helper.py` resides in the `xf_dsp/L2/meta` directory. To run it, change to the `xf_dsp` repository root and run the following command with your chosen options:

.. code-block::

python3 `xf_dsp/L2/meta/config_helper.py` [Options]
	--h [helper prints]
	--ip ip_name [providing the config helper the IP to configure]
	--mdir metadata_directory [by default config helper will guide you to `xf_dsp/L2/meta`]
	--outdir output_directory [by default config helper will guide you to `xf_dsp/L2/meta`]
	LIST_PARAMS [Lists the parameters of the chosen IP to configure]
	PRINT_GRAPH [Prints the resulting graph at the end of the configuration]
	NO_INSTANCE [no graph instance is to be generated at the end of the configuration]
	LIST_IPS [prints the IP list in DSPLIB]

Config Helper Example
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

    python3 xf_dsp/L2/meta/config_helper.py --ip hadamard LIST_PARAMS

.. _LEGALITY_CHECKING:

Legality Checking
^^^^^^^^^^^^^^^^^

Not all configurations of a given library unit are supported. For example, the valid range of ``TP_DECIMATE_FACTOR`` depends on the input sample data type ``TT_DATA``. Where possible, an unsupported configuration throws a ``static_assert`` compile-time error early in compilation. If the library element is generated from Vitis Model Composer, the error is reported there before generation. Error messages describe the cause — such as which parameters conflict — but the ``static_assert`` statement cannot include the specific configuration values.

It is not always possible to predict when a configuration exceeds the available resources on the target device (for example, memory). In such cases, the aiecompiler tool generates compile-time errors at a later stage.

Use :ref:`CONFIGURATION` to access the configuration utility, which guides you to a valid configuration before compilation.
