library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;
library WORK;
use WORK.FPGA_CONSTANT_PKG.ALL;

entity data_loader is
	port (
		clk : in std_logic;
		enable : in std_logic;
		reset : in std_logic;
		
		mem_addr : out std_logic_vector(RAM_DATA_ADDRESS_WIDTH - 1 downto 0);
		mem_write : out std_logic;
		mem_data : inout std_logic_vector(RAM_DATA_WORD_WIDTH - 1 downto 0);
		
		avr_data_in : in std_logic_vector(7 downto 0);
		avr_data_in_ready : in std_logic;
		avr_interrupt : out std_logic
	);
end data_loader;

architecture Behavioral of data_loader is

	signal running : std_logic := '0';
	signal toggle_in_value : std_logic := '0';
	signal data : std_logic_vector(RAM_DATA_WORD_WIDTH - 1 downto 0) := (others => '0');
	signal address : std_logic_vector(RAM_DATA_ADDRESS_WIDTH - 1 downto 0) := (others => '0');
	
	signal next_running : std_logic := '0';
	signal next_toggle_in_value : std_logic := '0';
	signal next_address : std_logic_vector(RAM_DATA_ADDRESS_WIDTH - 1 downto 0) := (others => '0');

begin
	
	update_internal_signals: process(running, avr_data_in_ready)
	begin
		if running = '0' and avr_data_in_ready /= toggle_in_value then
			next_running <= '1';
			next_toggle_in_value <= avr_data_in_ready;
		else
			next_running <= '0';
		end if;
	end process;
	
	clock_internal_signals: process(clk)
	begin
		if rising_edge(clk) then
			if reset = '1' then
				running <= '0';
				address <= (others => '0');
				toggle_in_value <= avr_data_in_ready;
				data <= (others => '0');
			else
				running <= next_running;
				address <= next_address;
				toggle_in_value <= next_toggle_in_value;
				data <= avr_data_in;
			end if;
		end if;
	end process;
	
	update_signals: process(running, address, avr_data_in)
	begin
		if running = '1' then
			mem_addr <= address;
			mem_data <= data;
			mem_write <= '0';
			avr_interrupt <= '1';
			
			next_address <= conv_std_logic_vector(unsigned(address) + 1, RAM_DATA_ADDRESS_WIDTH);
		else
			mem_addr <= (others => '0');
			mem_data <= (others => 'Z');
			mem_write <= '1';
			avr_interrupt <= '0';
			
			next_address <= address;
		end if;
	end process;

end Behavioral;

