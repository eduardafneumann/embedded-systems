	component SEProjetoFinalSingle is
		port (
			clk_clk : in std_logic := 'X'  -- clk
		);
	end component SEProjetoFinalSingle;

	u0 : component SEProjetoFinalSingle
		port map (
			clk_clk => CONNECTED_TO_clk_clk  -- clk.clk
		);

