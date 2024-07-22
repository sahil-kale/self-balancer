import os

def main(message_directory="str", nanopb_generator_path="str", output_dir="str"):
    # gather all the .proto files in the message directory, with the full path. Do so recursively.
    proto_files = []
    for root, _, files in os.walk(message_directory):
        for file in files:
            if file.endswith(".proto"):
                proto_files.append(os.path.join(root, file))

    # generate the .c/.h files from the .proto files with nanopb
    nanopb_args = [*proto_files, "-D", output_dir, f'--protoc-opt="--python_out={output_dir}"', '']
    print(f"{nanopb_generator_path} {' '.join(nanopb_args)}")
    os.system(f"{nanopb_generator_path} {' '.join(nanopb_args)}")

if __name__ == '__main__':
    message_directory = "messages"
    nanopb_generator_path = "nanopb/generator/nanopb_generator.py"
    output_dir = "generated"
    # make the output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    main(message_directory, nanopb_generator_path, output_dir=output_dir)