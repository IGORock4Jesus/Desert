using SharpDX;
using SharpDX.Direct3D11;
using SharpDX.DXGI;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using Buffer = SharpDX.Direct3D11.Buffer;
using Device = SharpDX.Direct3D11.Device;

namespace MeshEditor
{
	public partial class Form1 : Form
	{
		SwapChain swapChain;
		Device device;
		DeviceContext deviceContext;
		RenderTargetView renderTargetView;
		DepthStencilView depthStencilView;
		private Viewport viewport;
		InputElement[] inputElements;
		Buffer vertexBuffer, indexBuffer;
		VertexShader vertexShader;
		PixelShader pixelShader;
		Buffer vsb0, vsb1, psb0;
		InputLayout inputLayout;
		private bool enabled;
		private Task task;
		private Model model;

		public Form1()
		{
			InitializeComponent();
		}

		private void Form1_Load(object sender, EventArgs e)
		{
			InitializeCore();

			CreateModel();

			enabled = true;
			task = Task.Run(new Action(Rendering));
		}

		private void CreateModel()
		{
			ColladaLoader loader = new ColladaLoader();
			model = loader.Load(@"C:\Users\igoro\Documents\3dsMax\export\test1.DAE");
		}

		private void Rendering()
		{
			while (enabled)
			{
				deviceContext.Rasterizer.SetViewport(viewport);
				deviceContext.OutputMerger.SetTargets(depthStencilView, renderTargetView);

				deviceContext.ClearDepthStencilView(depthStencilView, DepthStencilClearFlags.Depth | DepthStencilClearFlags.Stencil, 1.0f, 0);
				deviceContext.ClearRenderTargetView(renderTargetView, SharpDX.Color.DarkSlateGray);

				Render();

				swapChain.Present(1, PresentFlags.None);

				Thread.Sleep(1);
			}
		}

		private void Render()
		{
			model.Render(device, deviceContext);
		}

		private void InitializeCore()
		{
			Device.CreateWithSwapChain(SharpDX.Direct3D.DriverType.Hardware, DeviceCreationFlags.Debug, new SwapChainDescription
			{
				BufferCount = 1,
				Flags = SwapChainFlags.None,
				IsWindowed = true,
				ModeDescription = new ModeDescription
				{
					Format = Format.R8G8B8A8_UNorm,
					Height = ClientSize.Height,
					RefreshRate = new Rational { Numerator = 1, Denominator = 60 },
					Scaling = DisplayModeScaling.Unspecified,
					ScanlineOrdering = DisplayModeScanlineOrder.Unspecified,
					Width = ClientSize.Width
				},
				OutputHandle = this.Handle,
				SampleDescription = new SampleDescription
				{
					Count = 1,
					Quality = 0
				},
				SwapEffect = SwapEffect.Discard,
				Usage = Usage.RenderTargetOutput
			}, out device, out swapChain);
			deviceContext = device.ImmediateContext;
			var texture = swapChain.GetBackBuffer<Texture2D>(0);
			renderTargetView = new RenderTargetView(device, texture);
			depthStencilView = new DepthStencilView(device, new Texture2D(device, new Texture2DDescription
			{
				ArraySize = 1,
				BindFlags = BindFlags.DepthStencil,
				Format = Format.D24_UNorm_S8_UInt,
				Height = ClientSize.Height,
				MipLevels = 1,
				CpuAccessFlags = CpuAccessFlags.None,
				OptionFlags = ResourceOptionFlags.None,
				SampleDescription = new SampleDescription(1, 0),
				Usage = ResourceUsage.Default,
				Width = ClientSize.Width
			}));
			viewport = new Viewport(0, 0, ClientSize.Width, ClientSize.Height, 0.0f, 1.0f);
		}

		private void Form1_FormClosed(object sender, FormClosedEventArgs e)
		{
			enabled = false;
			task.Wait();

			device.Dispose();
			deviceContext.Dispose();
			swapChain.Dispose();
			renderTargetView.Dispose();
			depthStencilView.Dispose();

		}

		private void Form1_Click(object sender, EventArgs e)
		{

		}
	}
}
