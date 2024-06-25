	component SEProjetoFinal is
		port (
			clk_clk : in std_logic := 'X'  -- clk
		);
	end component SEProjetoFinal;

	u0 : component SEProjetoFinal
		port map (
			clk_clk => CONNECTED_TO_clk_clk  -- clk.clk
		);

