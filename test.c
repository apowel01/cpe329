

static int fred = 71;


int my_func(int my_val)
{
	int fred = 2;
  int joe = my_val;

  return (fred + joe);
}

typedef struct {
	int val_1;
  int val_2;
  unsigned int val_4;
} t_my_struct_1;

typedef struct {
	unsigned int led_1;
  unsigned int led_2;   // assume led_1 starts in memory location 0 and led_2 starts in memory location 4 (if an int is 4 bytes)
  byte pad_1[12];				/* this is also a comment */
  unsigned int led_3;
} t_fpga_reg;

static t_fpga_reg *p_fpga_reg = 0x10000;

void led_1_set(unsigned int led_1_value)
{
	p_fpga_reg->led_1 = led_1_value;
}

unsigned int led_1_get(void)
{
	return p_fpga_reg->led_1;
}

int my_func_2(int my_val1, int my_val2)
{
	int fred = 2;
  int joe = my_val1 + my_val2;

  return (fred + joe);
}

int my_func_3(int my_val)
{
	my_val += 1;

  return (my_val);
}

int my_func_4(int *p_my_val)
{
	*p_my_val += 1; // change the value

  return (*p_my_val);
}

int my_func_5(t_my_struct_1 ms_1)
{
	ms_1.val_1 += 1; // change the value

  return (ms_1.val_2);
}

int my_func_6(t_my_struct_1 *p_ms_1)
{
	p_ms_1->val_1 += 1; // change the value

  return (p_ms_1->val_2);
}


int main(argc, argv)
{
	int return_value;
  int peter = 5;
	t_my_struct_1 atavan;
  t_my_struct_1 * p_atavan = &atavan;
  t_my_struct_1 ** pp_atavan = &p_atavan;


	return_value = my_func(peter);
  return_value += my_func_2(peter, fred);
  return_value = my_func_3(fred);
  return_value = my_func_4(&peter); // pass in the address of peter
  // peter is now 6, and return value is 6.
  atavan.val_1 = 5;
  atavan.val_2 = 7;
  atavan.val_4 = 100;

	return_value = my_func_5(atavan);
	return_value = my_func_6(&atavan);
  return_value = my_func_6(p_atavan);


}
